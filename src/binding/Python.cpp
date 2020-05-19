#include <unordered_map>

#include <shlublu/binding/Python.h>
#include <shlublu/binding/Python_ObjectHandlersCollection.h>

#include <shlublu/async/MutexLock.h>
#include <shlublu/text/String.h>


#ifdef _WIN32
#pragma warning( disable : 6285)
#endif


namespace shlublu
{

namespace Python
{
static std::unordered_map<std::string, ObjectPointer> __modules;
static std::unordered_map<ObjectPointer, std::unordered_map<std::string, ObjectPointer>> __callables;
static ObjectHandlersCollection __sObjects;


static wchar_t* __sArgv0(nullptr);
static MutexLock __sLock(false);


static void __throwException(std::string const& message)
{
    throw BindingException(message);
}


static void __shouldBeInitialized()
{
    if (!isInitialized())
    {
        __throwException("__shouldBeInitialized(): not in initialized state.");
    }
}


static void __decRef(ObjectHandler const& object)
{
    if (object.get()->ob_refcnt < 1)
    {
        __throwException("Python::__DecRef(): references count is already " + String::xtos(object.get()->ob_refcnt));
    }

    Py_DECREF(object);
}


static void __handleObjectUnregistration(ObjectHandler const& object, bool keepArg)
{
    if (keepArg)
    {
        Py_INCREF(object);
    }
    else if (__sObjects.isRegistered(object))
    {
        __sObjects.unregisterObject(object);
    }
}


const std::string moduleMain = "__main__";
const std::string moduleBuiltins = "builtins";


bool isInitialized()
{
    return __sArgv0 != nullptr;
}


void init(std::string const& programName, PathEntriesList const& pathList)
{
    MutexLock::Guard guard(__sLock);

    if (__sArgv0 == nullptr)
    {
        __sArgv0 = Py_DecodeLocale(programName.c_str(), nullptr);

        Py_SetProgramName(__sArgv0);
        Py_Initialize();
        
        if (!PyEval_ThreadsInitialized())
        {
            PyEval_InitThreads();
        }

        atexit(shutdown);

        execute(Program({ "import sys", "sys.path.append('.')" }));

        import(moduleMain);
        import(moduleBuiltins);
    }

    for (auto const& pathEntry : pathList)
    {
        execute("if '" + pathEntry + "' not in  sys.path:\n\tsys.path.append('" + pathEntry + "')");
    }
}


void shutdown()
{
    MutexLock::Guard guard(__sLock);

    if (__sArgv0)
    {
        for (auto& callableMapEntry : __callables)
        {
            for (auto& callableEntry : callableMapEntry.second)
            {
                __decRef(callableEntry.second);
            }
        }
        __callables.clear();

        for (auto& value : __sObjects)
        {
            __decRef(value);
        }
        __sObjects.clear();

        for (auto& moduleEntry : __modules)
        {
            __decRef(moduleEntry.second);
        }
        __modules.clear();

        Py_Finalize();
        PyMem_RawFree(__sArgv0);

        __sArgv0 = nullptr;
    }
}


void execute(RawCode const& code)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    if (PyRun_SimpleString(code.c_str()) < 0)
    {
        __throwException("Python::execute(): Instruction '" + code + "' caused an error");
    }
}


void execute(Program const& program)
{
    RawCode code;

    for (auto const& line : program)
    {
        code += line + '\n';
    }

    execute(code);
}


void beginCriticalSection()
{
    __sLock.lock();

    try
    {
        __shouldBeInitialized();
    }
    catch (std::exception const& e)
    {
        __sLock.unlock();
        throw e;
    }
}


void endCriticalSection()
{
    __sLock.unlock();
}


ObjectPointer import(std::string const& moduleName)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    ObjectPointer pythonModule(nullptr);

    if (!__modules.count(moduleName))
    {
        const auto pythonModuleName(PyUnicode_DecodeFSDefault(moduleName.c_str()));
        pythonModule = PyImport_Import(pythonModuleName);
        __decRef(pythonModuleName);

        if (!pythonModule)
        {
            __throwException("Python::import(): Cannot import module '" + moduleName + "'");
        }

        __modules.emplace(moduleName, pythonModule);
    }
    else
    {
        pythonModule = __modules.at(moduleName);
    }

    return pythonModule;
}


ObjectPointer module(std::string const& moduleName)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    if (!__modules.count(moduleName))
    {
        __throwException("Python::module(): Cannot retrieve '" + moduleName + "' in imported modules");
    }

    auto const& ret(__modules.at(moduleName));

    return ret;
}


ObjectHandler const& object(ObjectPointer scope, std::string const& objectName)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    const ObjectHandler pythonObject(PyObject_GetAttrString(scope, objectName.c_str()));

    if (!pythonObject.get())
    {
        __throwException("Python::object(): Cannot access to object '" + objectName + "'");
    }

    return __sObjects.registerObject(pythonObject);
}


ObjectHandler const& object(std::string const& moduleName, std::string const& objectName)
{
    return object(module(moduleName), objectName);
}


ObjectPointer callable(ObjectPointer scope, std::string const& callableName, bool forceReload)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    ObjectPointer pythonCallable(nullptr);

    if (!__callables.count(scope) || !__callables.at(scope).count(callableName) || forceReload)
    {
        pythonCallable = PyObject_GetAttrString(scope, callableName.c_str());

        if (!pythonCallable)
        {
            __throwException("Python::callable(): Cannot access to callable '" + callableName + "'");
        }

        if (!PyCallable_Check(pythonCallable))
        {
            __throwException("Python::callable(): '" + callableName + "' is not callable");
        }

        if (__callables.count(scope) && __callables.at(scope).count(callableName))
        {
            __decRef(callable(scope, callableName, false));
        }

        __callables[scope][callableName] = pythonCallable;
    }
    else
    {
        pythonCallable = __callables.at(scope).at(callableName);
    }

    return pythonCallable;
}


ObjectPointer callable(std::string const& moduleName, std::string const& callableName, bool forceReload)
{
    return callable(module(moduleName), callableName, forceReload);
}


ObjectHandler const& call(ObjectPointer callableObject, ObjectHandlersList const& args, bool keepArguments)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    const auto pyArgsTuple(args.size() ? tuple(args, keepArguments) : nullptr);
    const auto pyRet(PyObject_CallObject(callableObject, pyArgsTuple));
    
    if (pyArgsTuple)
    {
        forgetArgument(pyArgsTuple);
    }

    if (!pyRet)
    {
        __throwException("Python::call(): failure in calling callable");
    }

    return __sObjects.registerObject(pyRet);
}


ObjectHandler tuple(ObjectHandlersList const& args, bool keepArguments)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    const ObjectHandler tuple(PyTuple_New(args.size()));

    if (!tuple)
    {
        __throwException("Python::tuple(): failure in creating tuple");
    }

    const auto& ret(__sObjects.registerObject(tuple));

    size_t pos(0);

    for (auto const& object : args)
    {
        __handleObjectUnregistration(object, keepArguments);

        PyTuple_SetItem(tuple, pos++, object);
    }

    return ret;
}


ObjectHandler list(ObjectHandlersList const& args, bool keepArguments)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    const ObjectHandler list(PyList_New(args.size()));

    if (!list)
    {
        __throwException("Python::list(): failure in creating list");
    }

    const auto& ret(__sObjects.registerObject(list));

    size_t pos(0);

    for (auto const& object : args)
    {
        __handleObjectUnregistration(object, keepArguments);

        PyList_SetItem(list, pos++, object);
    }

    return ret;
}


void addList(ObjectHandler objList, ObjectHandler item, bool keepArguments)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    if (!PyList_Check(objList))
    {
        __throwException("Python::addList(): Trying to add an item to an object that is not a list");
    }

    __handleObjectUnregistration(item, keepArguments);

    PyList_Append(objList, item);
    __decRef(item);
}


ObjectHandler fromAscii(std::string const& str)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    const auto object(PyUnicode_FromWideChar(String::toWString(str).c_str(), -1));

    return __sObjects.registerObject(object);
}


std::string toAscii(ObjectHandler object, bool keepArg)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    if (!PyUnicode_Check(object))
    {
        __throwException("Python::toAscii(): Trying to convert an object that is not a Unicode string to an ASCII string");
    }

    wchar_t* wstr(PyUnicode_AsWideCharString(object, nullptr));
    const std::string ret(String::fromWString(wstr));
    PyMem_Free(wstr);

    __handleObjectUnregistration(object, keepArg);

    if (!keepArg)
    {
        __decRef(object);
    }

    return ret;
}


ObjectHandler const& keepArgument(ObjectHandler const& object)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    if (!__sObjects.isRegistered(object))
    {
        __throwException("Python::keepArgument(): object is not under control");
    }

    Py_INCREF(object);

    const ObjectHandler newHandler(object.get());
    
    return __sObjects.registerObject(newHandler);
}


ObjectHandler const& controlArgument(ObjectHandler object)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    if (__sObjects.isRegistered(object))
    {
         __throwException("Python::controlArgument(): Trying to give control of an object that is already under control");
    }

    return __sObjects.registerObject(object);
}


void forgetArgument(ObjectHandler const& object)
{
    MutexLock::Guard guard(__sLock);
    __shouldBeInitialized();

    if (!__sObjects.isRegistered(object))
    {
        __throwException("Python::forgetArgument(): Trying to forget an object that is not under control");
    }

    __decRef(object);
    __sObjects.unregisterObject(object);
}

}

}

