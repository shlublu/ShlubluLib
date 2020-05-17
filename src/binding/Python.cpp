#include<shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();

#include <unordered_map>
#include <unordered_set>

#include <shlublu/binding/Python.h>

#include <shlublu/async/MutexLock.h>
#include <shlublu/text/String.h>

#include <shlublu/util/Debug.h>

#ifdef _WIN32
#pragma warning( disable : 6285)
#endif


// Useful documentation:
// https://docs.python.org/fr/3/extending/embedding.html#embedding-python-in-c
// https://docs.python.org/3/c-api/


static shlublu::MutexLock __pythonLock(false);

static wchar_t* __pythonArgv0(nullptr);

static std::unordered_map<std::string, shlublu::Python::ScopeRef> __pythonModules;
static std::unordered_map<shlublu::Python::ScopeRef, std::unordered_map<std::string, shlublu::Python::CallableRef>> __pythonCallables;

static std::unordered_set<shlublu::Python::ObjectHandler, shlublu::Python::ObjectHandler::Hasher> __pythonObjects;


static void __pythonGrab()
{
    __pythonLock.queueLock();
}


static void __pythonRelease()
{
    __pythonLock.unlock();
}


static void __pythonThrowException(std::string const& message)
{
    __pythonRelease();
 
    throw shlublu::Python::BindingException(message);
}


static void __pythonShouldBeInitialized()
{
    __pythonGrab();

    if (!__pythonArgv0)
    {
        __pythonThrowException("__pythonShouldBeInitialized(): not in initialized state.");
    }
}


static void __pythonWithdrawAsCatched(shlublu::Python::ObjectHandler const& object)
{
    if (__pythonObjects.count(object))
    {
        __pythonObjects.erase(object);
    }
}


static void __pythonDecRef(shlublu::Python::ObjectHandler const& object)
{
    if (!object.get()->ob_refcnt)
    {
        __pythonThrowException("Python::loseArgument(): references count is already zero");
    }

    Py_DECREF(object);
}


namespace shlublu
{

const std::string Python::moduleMain = "__main__";
const std::string Python::moduleBuiltins = "builtins";


bool Python::isInitialized()
{
    return __pythonArgv0 != nullptr;
}


void Python::init(std::string const& programName, PathEntriesList const& pathList)
{
    __pythonGrab();

    if (__pythonArgv0 == nullptr)
    {
        __pythonArgv0 = Py_DecodeLocale(programName.c_str(), nullptr);

        Py_SetProgramName(__pythonArgv0);
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

    __pythonRelease();
}


void Python::shutdown()
{
    SHLUBLU_TODO("References count deserves a unit test suite");

    __pythonGrab();

    if (__pythonArgv0)
    {
        for (auto& callableMapEntry : __pythonCallables)
        {
            for (auto& callableEntry : callableMapEntry.second)
            {
                __pythonDecRef(callableEntry.second);
            }
        }
        __pythonCallables.clear();

        for (auto& value : __pythonObjects)
        {
            __pythonDecRef(value);
        }
        __pythonObjects.clear();

        for (auto& moduleEntry : __pythonModules)
        {
            __pythonDecRef(moduleEntry.second);
        }
        __pythonModules.clear();

        Py_Finalize();
        PyMem_RawFree(__pythonArgv0);

        __pythonArgv0 = nullptr;
    }

    __pythonRelease();
}


void Python::execute(RawCode const& code)
{
    __pythonShouldBeInitialized();

    if (PyRun_SimpleString(code.c_str()) < 0)
    {
        __pythonThrowException("Python::execute(): Instruction '" + code + "' caused an error");
    }

    __pythonRelease();
}


void Python::execute(Program const& program)
{
    RawCode code;

    for (auto const& line : program)
    {
        code += line + '\n';
    }

    execute(code);
}


void Python::beginCriticalSection()
{
    __pythonShouldBeInitialized();
}


void Python::endCriticalSection()
{
    __pythonRelease();
}


Python::ScopeRef Python::import(std::string const& moduleName)
{
    __pythonShouldBeInitialized();

    ScopeRef pythonModule(nullptr);

    if (!__pythonModules.count(moduleName))
    {
        const auto pythonModuleName(PyUnicode_DecodeFSDefault(moduleName.c_str()));
        pythonModule = PyImport_Import(pythonModuleName);
        __pythonDecRef(pythonModuleName);

        if (!pythonModule)
        {
            __pythonThrowException("Python::import(): Cannot import module '" + moduleName + "'");
        }

        __pythonModules.emplace(moduleName, pythonModule);
    }
    else
    {
        pythonModule = __pythonModules.at(moduleName);
    }

    __pythonRelease();

    return pythonModule;
}


Python::ScopeRef Python::module(std::string const& moduleName)
{
    __pythonShouldBeInitialized();

    if (!__pythonModules.count(moduleName))
    {
        __pythonThrowException("Python::module(): Cannot retrieve '" + moduleName + "' in imported modules");
    }

    auto const& ret(__pythonModules.at(moduleName));

    __pythonRelease();

    return ret;
}


Python::ObjectHandler const& Python::object(ScopeRef scopeRef, std::string const& objectName)
{
    __pythonShouldBeInitialized();

    const ObjectHandler pythonObject(PyObject_GetAttrString(scopeRef, objectName.c_str()));

    if (!pythonObject.get())
    {
        __pythonThrowException("Python::object(): Cannot access to object '" + objectName + "'");
    }

    const auto& ret(*__pythonObjects.emplace(pythonObject).first);

    __pythonRelease();

    return ret;
}


Python::ObjectHandler const& Python::object(std::string const& moduleName, std::string const& objectName)
{
    return object(module(moduleName), objectName);
}


Python::CallableRef Python::callable(ScopeRef scopeRef, std::string const& callableName, bool forceReload)
{
    __pythonShouldBeInitialized();

    CallableRef pythonCallable(nullptr);

    if (!__pythonCallables.count(scopeRef) || !__pythonCallables.at(scopeRef).count(callableName) || forceReload)
    {
        pythonCallable = PyObject_GetAttrString(scopeRef, callableName.c_str());

        if (!pythonCallable)
        {
            __pythonThrowException("Python::callable(): Cannot access to callable '" + callableName + "'");
        }

        if (!PyCallable_Check(pythonCallable))
        {
            __pythonThrowException("Python::callable(): '" + callableName + "' is not callable");
        }

        if (__pythonCallables.count(scopeRef) && __pythonCallables.at(scopeRef).count(callableName))
        {
            __pythonDecRef(callable(scopeRef, callableName, false));
        }

        __pythonCallables[scopeRef][callableName] = pythonCallable;
    }
    else
    {
        pythonCallable = __pythonCallables.at(scopeRef).at(callableName);
    }

    __pythonRelease();

    return pythonCallable;
}


Python::CallableRef Python::callable(std::string const& moduleName, std::string const& callableName, bool forceReload)
{
    return callable(module(moduleName), callableName, forceReload);
}


Python::ObjectHandler const& Python::arguments(ObjectHandlersList const& args)
{
    __pythonShouldBeInitialized();

    const ObjectHandler argsTuple(args.size() ? PyTuple_New(args.size()) : nullptr);

    if (argsTuple)
    {
        __pythonObjects.emplace(argsTuple);

        size_t pos(0);

        for (auto const& object : args)
        {
            PyTuple_SetItem(argsTuple, pos++, object);
            __pythonWithdrawAsCatched(object);
        }
    }
    else
    {
        __pythonThrowException("Python::arguments(): failure in creating tuple");
    }

    __pythonRelease();

    return *__pythonObjects.find(argsTuple);
}


Python::ObjectHandler const& Python::call(CallableRef callableObject, ObjectHandler argumentsObject, bool keepArguments)
{
    SHLUBLU_TODO("Parameter keepArguments deserves a unit test");

    __pythonShouldBeInitialized();

    if (argumentsObject && !PyTuple_Check(argumentsObject))
    {
        __pythonThrowException("Python::call(): argumentsObject is not a tuple");
    }

    const auto pyRet(PyObject_CallObject(callableObject, argumentsObject));
    
    if (argumentsObject && !keepArguments)
    {
        forgetArgument(argumentsObject);
    }

    if (!pyRet)
    {
        __pythonThrowException("Python::call(): failure in calling callable");
    }

    const auto& ret(*__pythonObjects.emplace(pyRet).first);

    __pythonRelease();

    return ret;
}


Python::ObjectHandler Python::tuple(ObjectHandlersList const& args)
{
    __pythonShouldBeInitialized();

    const ObjectHandler objTuple(PyTuple_New(args.size()));

    size_t pos(0);

    for (auto const& object : args)
    {
        PyTuple_SetItem(objTuple, pos++, object);
        __pythonWithdrawAsCatched(object);
    }

    __pythonRelease();

    return objTuple;
}


Python::ObjectHandler Python::list(ObjectHandlersList const& args)
{
    __pythonShouldBeInitialized();

    const ObjectHandler objList(PyList_New(args.size()));

    size_t pos(0);

    for (auto const& object : args)
    {
        PyList_SetItem(objList, pos++, object);
        __pythonWithdrawAsCatched(object);
    }

    __pythonRelease();

    return objList;
}


void Python::addList(ObjectHandler objList, ObjectHandler item, bool keepArguments)
{
    SHLUBLU_TODO("Parameter keepArguments deserves a unit test");

    __pythonShouldBeInitialized();

    if (PyList_Check(objList))
    {
        PyList_Append(objList, item);

        if (!keepArguments)
        {
            __pythonDecRef(item);
            __pythonWithdrawAsCatched(item);
        }
    }
    else
    {
        __pythonThrowException("Python::addList(): Trying to add an item to an object that is not a list");
    }

    __pythonRelease();
}


Python::ObjectHandler Python::fromAscii(std::string const& str)
{
    __pythonShouldBeInitialized();

    const auto ret(PyUnicode_FromWideChar(String::toWString(str).c_str(), -1));

    __pythonRelease();

    return ret;
}


std::string Python::toAscii(ObjectHandler object, bool keepArgument)
{
    SHLUBLU_TODO("Parameter keepArguments deserves a unit test");

    __pythonShouldBeInitialized();

    std::string ret;

    if (PyUnicode_Check(object))
    {
        wchar_t* wstr(PyUnicode_AsWideCharString(object, nullptr));
        ret = String::fromWString(wstr);
        PyMem_Free(wstr);

        if (!keepArgument)
        {
            __pythonDecRef(object);
            __pythonWithdrawAsCatched(object);
        }
    }
    else
    {
        __pythonThrowException("Python::toAscii(): Trying to convert an object that is not a Unicode string to an ASCII string");
    }

    __pythonRelease();

    return ret;
}


Python::ObjectHandler const& Python::keepArgument(ObjectHandler const& object)
{
    __pythonShouldBeInitialized();

     if (!__pythonObjects.count(object))
    {
        __pythonThrowException("Python::keepArgument(): Argument is not under control");
    }

    Py_INCREF(object);

    const ObjectHandler newHandler(object.get());
    const auto& ret(*__pythonObjects.emplace(newHandler).first);

    __pythonRelease();

    return ret;
}


Python::ObjectHandler const& Python::controlArgument(ObjectHandler object)
{
    __pythonShouldBeInitialized();

     if (__pythonObjects.count(object))
    {
         __pythonThrowException("Python::controlArgument(): Trying to give control of an object that is already under control");
    }

     const auto& ret (*__pythonObjects.emplace(object).first);

    __pythonRelease();

    return ret;
}


void Python::forgetArgument(ObjectHandler const& object)
{
    __pythonShouldBeInitialized();

    if (__pythonObjects.count(object))
    {
        __pythonDecRef(object);
        __pythonObjects.erase(object);
    }
    else
    {
        __pythonThrowException("Python::forgetArgument(): Trying to forget an object that is not under control");
    }

    __pythonRelease();
}

}
