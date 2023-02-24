#pragma once

/** @file
	Main file of the Python module.
	Based on the <a href="https://docs.python.org/3/c-api/index.html">CPython standard API</a>, this module is intended to make Python integration easier.

	See Python namespace documentation for details.
*/

#include <string>
#include <vector>

#include <shlublu/binding/Python_BindingExceptions.h>
#include <shlublu/binding/Python_ObjectHandler.h>


namespace shlublu
{

/** @namespace shlublu::Python
	Helper classes and functions wrapping the <a href="https://docs.python.org/3/c-api/index.html">CPython standard API</a>.

	Not all functions of the CPython API are wrapped here. shlublu::Python is intended to make the most common operations simpler without preventing
	users from using advanced features from the CPython API when needed. In particular, this module focuses of making the references count handling simpler
	and less error-prone than <a href="https://docs.python.org/3/extending/extending.html">doing it manually as CPython requires</a>.

	<b>Requirements</b><br />
	This module uses the official `Python3.x` library.
	Client programs need their include path, libraries path and linker input to be updated as follows:
	<ul>
	<li>Include path: `<depends on your local installation>`</li>
	<li>Libraries path: `<depends on your local installation>`</li>
	<li>Library file:
		<ul>
			<li>Linux: `libpython3.<x>m.a`</li>
			<li>Windows: `python3<x>.dll`</li>
		</ul>
	</li>
	</ul>

	<b>Typical examples of use</b>
	
	<ul>
	<li>Should you just need to execute some code with the CPython Interpreter:
	@code
	#include <iostream>
	#include <vector>

	#include <shlublu/binding/Python.h>
	#include <shlublu/text/String.h>

	using namespace shlublu;

	int main(int, char* argv[])
	{
		int retCode(0);

		try
		{
			const int x(5);
			const double y(2.0);

			// Initializes the Python Interpreter and gives it the name of this program.
			Python::init(*argv);

			// Writes a function: sum(x,y) -> x + y, then prints the result of a call 
			Python::execute("def sum(x, y):\n\treturn x + y");
			Python::execute("print('Sum: ' + repr(sum(" + String::xtos(x) + "," + String::xtos(y) + ")))");

			// Instructions can also be executed as a program
			const Python::Program program({ "import math", "print('Factorial: ' + repr(math.factorial(" + String::xtos(x) + ")))" });
			Python::execute(program);

			Python::shutdown(); // optional: this will be called atexit() anyway
		}
		catch (std::exception const& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
			retCode = -1;
		}

		return retCode;
	}
	@endcode
	</li>
	<li>Should you need your program to interact more deeply with the CPython Interpreter:
	@code
	#include <iostream>
	#include <vector>

	#include <shlublu/binding/Python.h>
	#include <shlublu/text/String.h>

	using namespace shlublu;

	int main(int, char* argv[])
	{
		int retCode(0);

		try
		{
			const double x(5);
			const int y(2);

			Python::init(*argv);

			// Imports math and keeps a pointer on it for later
			const auto math(Python::import("math"));

			// Calls 'math.pow(y, x)' using Python and some CPython functions it would have been pointless to wrap. Keeps the result as a Python object called 'xPowY'
			const auto xPowY
			(
				Python::call
				(
					Python::callable(math, "pow"),                  // takes a pointer to math.pow
					{ PyFloat_FromDouble(x), PyLong_FromLong(y) }	// passes arguments obtained from CPython calls
				)
			);

			// Python::call() has decreased the references counts of the arguments for you. You would have had to do it manually otherwise.
			// Should you need to keep them for later there are ways. This is explained later in this page.

			// Prints the result using std::iostream and CPython
			std::cout << "pow(" << x << ", " << y << "): " << PyFloat_AsDouble(xPowY) << std::endl; // displays "pow(5, 2): 25"

			// Now let's compute and print the square root of this result using the interpreter (this displays "5.0")
			Python::call
			(
				Python::callable(Python::moduleBuiltins, "print"),  // takes a pointer to the Python built-in 'print' function

				{
					Python::call
					(
						Python::callable("math", "sqrt"),			// this shows that keeping a pointer on imports is not mandatory. Using names is ok too as long as the import has been done.
						{ xPowY }									// passes the result of the previous call to 'pow' as an argument to 'sqrt'
					)
				}
			);

			// Python::shutdown() will be called atexit(). This will take care of decreasing all needed references counts for you (modules, functions, arguments, results, etc).
			// There are ways to do some cleanup manually should memory be an issue. This is explained later in this page.
		}
		catch (std::exception const& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
			retCode = -1;
		}

		return retCode;
	}
	@endcode
	</li>
	<li>More advanced features are explained below.</li>
	</ul>

	@attention <b>About concurrency</b><br/>
	The CPython interpreter is global and shared by all the threads of the process. So is its memory state, including imports and objects.<br /> 
	All functions of this module are thread-safe. However, 
	<ul>
	<li>groups of Python calls that constitute transactions in your C++ code should be surrounded by `beginCriticalSection()` / `endCriticalSection()` to prevent 
	other threads from calling the CPython interpreter while these transactions	are executing. Such interruptions would not crash the program but could make the results
	inconsistents should these threads work on the same shared pieces of data.</li>
	<li>calls to CPython functions that access to objects should also be surrounded by `beginCriticalSection()` / `endCriticalSection()` as these functions 
	do not support concurrency. For example, `PyLong_FromLong(42)`, `Py_XDECREF(object)`, or `PyLong_AsLong(object)` fall in this category.</li>
	</ul>
	Should you need more isolation, you can use the CPython API to setup
	<a href="https://docs.python.org/3/c-api/init.html#c.Py_NewInterpreter">multiple interpreters</a> knowing that there are caveats
	described in the documentation. One of this limitation is <a href="https://docs.python.org/3/c-api/init.html#non-python-created-threads">the use of 
	the `PyGILState_*()` API</a>. As shlublu::Python doesn't use this API there should not be any issue there.<br /><br />
	Forking is ok as the child process receives a full copy of the memory of the parent process made at `fork()` time.
	Testing shows the interpreter supports this very well, providing a full isolation, as long as the fork operation
	is conducted from a mono-thread process. Should you wish to fork a multi-threads process
	<a href="https://docs.python.org/3/c-api/init.html#cautions-about-fork">there are some cautions</a>.
*/
namespace Python
{
	using PathEntriesList = std::vector<std::string>; /**< Path, as a vector of strings. */
	using ObjectHandlersList = std::vector<ObjectHandler>; /**< Parameters list to pass to `call()` or to functions that create collections. */

	using ObjectPointer = PyObject*; /**<  Pointer to scope objects (either imported modules or instances of a class) or callable objects (functions or methods). Conversion from and to ObjectHandler is silent. */

	using RawCode = std::string; /**< Plain Python code. */
	using Program = std::vector<RawCode>; /**< Complete program. Typical use is one line per element. Indentation is under the responsability of the programmer. */

	extern const std::string moduleMain; /**< Main module ("__main__"). Imported automatically at `init()` time. */
	extern const std::string moduleBuiltins; /**< Built-ins module ("builtins"). Imported automatically at `init()` time. */

	/**
		Check whether Python is initialized.
		Calling `shutdown()` makes Python not to be initialized anymore.
		@return the initialization status
	*/
	bool isInitialized();

	/**
		Initializes Python.
		Should `pythonSysPath` differ between two calls not separated by a `shutdown()`, elements of the second path that are not part of the first are appended.

		This function can be called several times in a row with no issue.

		@param programName the name to give the interpreter. The `argv[0]` argument given to the `main()` function of your program 
		<a href="https://docs.python.org/3/c-api/init.html#c.Py_SetProgramName">is a preferred choice</a> though not mandatory (see below)
		@param pythonSysPath system path that will be appended to `sys.path` if not already part of it
		@see <a href="https://docs.python.org/3/c-api/init.html#c.Py_SetProgramName">Py_SetProgramName()</a>
	*/
	void init(std::string const& programName, PathEntriesList const& pythonSysPath = PathEntriesList());

	/**
		Shuts down Python.
		Cleans up all references to the various objects previously handled or created by using Python and calls `Py_Finalize()`.

		This function can be called several times in a row with no issue even if Python has not been initialized in the first place.
		It is called through `atexit()`, which makes its explicit use optional.

		@see <a href="https://docs.python.org/3/c-api/init.html?highlight=py_finalize#c.Py_Finalize">Py_Finalize()</a>
		@see <a href="https://www.cplusplus.com/reference/cstdlib/atexit/">atexit()</a>
	*/
	void shutdown();

	/**
		Executes raw code.

		@param code piece of code to execute. Lines should be separated by `\n`. Intentation should be materialized by spaces or `\t`.
		@exception BindingLogicError if Python is not initialized or if the piece of code causes an error at interpretation time.

		<b>Example</b>
		@code
		Python::execute("print('text to print')"); // prints "text to print"
		@endcode
	*/
	void execute(RawCode const& code);

	/**
		Executes a program.
		The whole program is executed in a row and cannot be interrupted by other threads using the CPython interpreter.
		
		@param program code to execute, typically splitted by lines. Intentation should be materialized by spaces or `\t`. Empty lines are permitted.
		@exception BindingLogicError if Python is not initialized or if any line of code causes an error at interpretation time.

		<b>Example</b>
		@code
		Python::execute({ "def testFunc(x):", "\tprint(x + ' is now printed')", "testFunc('text to print')" }); // prints "text to print is now printed"
		@endcode
	*/
	void execute(Program const& program);


	/**
		Imports a module by its name.
		Modules can be imported several times in a row with no issue, the same pointer being returned each time.

		The returned pointer is under control of Python for garbage collection at `shutdown()` time only. 

		@param moduleName the name of the module, dot-delimited should it be part of a package
		@return a pointer to the imported module
		@exception BindingLogicError if Python is not initialized or if the module cannot be found
		
		<b>Example</b>
		@code
		const auto pathModule(Python::import("os.path"));
		@endcode
	*/
	ObjectPointer import(std::string const& moduleName);


	/**
		Retrieves a previously imported module.
		Modules have to be imported by `import()` to be retrieved that way. Modules imported by `execute()` (as `execute("import <name>")`) are
		not retrieved by this function.

		The returned pointer is under control of Python for garbage collection at `shutdown()` time only.

		@param moduleName the name of the module, dot-delimited should it be part of a package
		@return a pointer to the module
		@exception BindingLogicError if Python is not initialized or if the module has not be previsouly imported by `import()` 
		
		<b>Example</b>
		@code
		Python::import("os.path")
		const auto pathModule(Python::retrieve("os.path"));
		@endcode
	*/
	ObjectPointer module(std::string const& moduleName);


	/**
		Retrieves an object by its name from a scope pointer.

		The returned handler is under control of Python for garbage collection. 
		Such a garbage collection is triggered by several functions. This is mentioned in their documentation.

		@param scope pointer to the module, namespace or object the object to retrieve belongs to
		@param objectName the name of the object to retrieve
		@return a handler of the retrieved object
		@exception BindingLogicError if Python is not initialized or if the object cannot be found

		<b>Example</b>
		@code
		Python::execute(Python::Program({ "import fractions as f", "fraction = f.Fraction(3, 2)" }));

		const auto fraction(Python::object(Python::moduleMain, "fraction"));
		const auto denominator(Python::object(fraction, "denominator"));

		std::cout << String::xtos(PyLong_AsLong(denominator)) << std::endl;
		@endcode
	*/
	ObjectHandler const& object(ObjectPointer scope, std::string const& objectName);


	/**
		Retrieves an object by its name from a module name.

		The returned handler is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions. This is mentioned in their documentation.

		@param moduleName the name of the module the object to retrieve belongs to
		@param objectName the name of the object to retrieve
		@return a handler of the retrieved object
		@exception BindingLogicError if Python is not initialized, if the module has not been imported by `import()`, or if the object cannot be found

		<b>Example</b>
		@code
		Python::execute(Python::Program({ "import fractions as f", "fraction = f.Fraction(3, 2)"}));

		const auto fraction(Python::object(Python::moduleMain, "fraction"));
		@endcode
	*/
	ObjectHandler const& object(std::string const& moduleName, std::string const& objectName);

	/**
		Retrieves a callable object (function or method) by its name from a scope pointer.
		Callable objects are less likely to change than non-callable objects during the execution of the program. 
		For this reason, pointers to callable are memorized. Multiple calls to this function with the same scope argument and 
		callable name make the pointer retrieved in the first place to be returned, unless `forceReload` is set to true.

		The returned pointer is under control of Python for garbage collection at `shutdown()` time only.

		@param scope pointer to the module, namespace or object the callable object to retrieve belongs to
		@param callableName the name of the callable object to retrieve
		@param forceReload if true, refresh the callable object pointer before returning it and dispose of any previous version
		@return a pointer to the retrieved callable object
		@exception BindingLogicError if Python is not initialized, if the object cannot be found or if it is not callable

		<b>Example</b>
		@code
		const auto mathModule(Python::import("math"));
		const auto fabs(Python::callable(mathModule, "fabs"));

		std::cout << PyFloat_AsDouble(Python::call(fabs, { PyFloat_FromDouble(-42.5) })) << std::endl;
		@endcode
	*/
	ObjectPointer callable(ObjectPointer scope, std::string const& callableName, bool forceReload = false);

	/**
		Retrieves a callable object (function) by its name from a module name.
		Callable objects are less likely to change than non-callable objects during the execution of the program.
		For this reason, pointers to callable are memorized. Multiple calls to this function with the same scope argument and
		callable name make the pointer retrieved in the first place to be returned, unless `forceReload` is set to true.

		The returned pointer is under control of Python for garbage collection at `shutdown()` time only.

		@param moduleName name of the module the callable object to retrieve belongs to
		@param callableName the name of the callable object to retrieve
		@param forceReload if true, refresh the callable object pointer before returning it and dispose of any previous version
		@return a pointer to the retrieved callable object
		@exception BindingLogicError if Python is not initialized, if the module has not been imported by `import()`, if the object cannot be found or if it is not callable

		<b>Example</b>
		@code
		Python::import("math");
		const auto fabs(Python::callable("math", "fabs"));

		std::cout << PyFloat_AsDouble(Python::call(fabs, { PyFloat_FromDouble(-42.5) })) << std::endl;
		@endcode
	*/
	ObjectPointer callable(std::string const& moduleName, std::string const& callableName, bool forceReload = false);

	/**
		Calls a callable with the given positional arguments.

		Once the callable returns, the references counts of the elements of `args` are decreased unless `keepArguments` is true in which case none are decreased. 
		Should you wich to only keep some of the passed arguments, `keepArguments` should be set to `false` and `controlArgument()`/`keepArgument()` should be used 
		with the arguments to preserve.

		The returned handler is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions. This is mentioned in their documentation.

		@param callableObject pointer to the callable object to call, typically returned by `callable()`
		@param args arguments of the call. They can either be obtained from previous Python calls or from CPython calls. Empty if no aregument is required.
		@param keepArguments if true, the references counts of the elements of `args` will not be decreased so they can be reused later on
		@return a handler of the result of the call, which is `None` if the callable returns no value.
		@exception BindingLogicError if Python is not initialized
		@exception BindingRuntimeError if any issue occurs during the call. Should this happen the references counts of the elements of `args` will have been decreased anyway.

		<b>Example</b>
		@code
		const auto print(Python::callable(Python::moduleBuiltins, "print"));

		Python::call(print, { Python::fromAscii("text to print") });
		@endcode
	*/
	ObjectHandler const& call(ObjectPointer callableObject, ObjectHandlersList const& args = {}, bool keepArguments = false);

	/**
		Creates a tuple object initialized with the given arguments.
		This function <a href="https://docs.python.org/3/extending/extending.html#reference-counts">steals a reference</a> from each element of `args` unless `keepArguments` 
		is true in which case they are all preserved.
		Should you wich to only preserve some of the passed arguments, `keepArguments` should be set to `false` and `controlArgument()`/`keepArgument()` should be used
		with the arguments to preserve.

		The returned handler is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions. This is mentioned in their documentation.

		@param args initializer of the tuple. These arguments can either be obtained from previous Python calls or from CPython calls. Empty to create an empty tuple.
		@param keepArguments if true, no references will be stolen from `args`
		@return a handler of the created tuple object
		@exception BindingLogicError if Python is not initialized
		@exception BindingRuntimeError if any issue occurs during the creation of the tuple

		<b>Example</b>
		@code
		const auto tuple(Python::tuple({ PyLong_FromLong(42), Python::fromAscii("42") }));

		// Prints the length of the tuple ("2")
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			{
				Python::call(Python::callable(Python::moduleBuiltins, "len"), { tuple })
			}
		);
		@endcode
	*/
	ObjectHandler tuple(ObjectHandlersList const& args = {}, bool keepArguments = false);
	
	/**
		Creates a list object initialized with the given arguments.
		This function <a href="https://docs.python.org/3/extending/extending.html#reference-counts">steals a reference</a> from each element of `args` unless `keepArguments`
		is true in which case they are all preserved.
		Should you wich to only preserve some of the passed arguments, `keepArguments` should be set to `false` and `controlArgument()`/`keepArgument()` should be used
		with the arguments to preserve.

		The returned handler is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions. This is mentioned in their documentation.

		@param args initializer of the list. These arguments can either be obtained from previous Python calls or from CPython calls. Empty to create an empty list.
		@param keepArguments if true, no references will be stolen from `args`
		@return a handler of the created list object
		@exception BindingLogicError if Python is not initialized
		@exception BindingRuntimeError if any issue occurs during the creation of the list

		<b>Example</b>
		@code
		const auto list(Python::list({ PyLong_FromLong(42), Python::fromAscii("42") }));

		// Prints the length of the list ("2")
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			{
				Python::call(Python::callable(Python::moduleBuiltins, "len"), { list })
			}
		);
		@endcode
	*/
	ObjectHandler list(ObjectHandlersList const& args = {}, bool keepArguments = false);

	/**
		Adds an item to the end of a list.
		This function <a href="https://docs.python.org/3/extending/extending.html#reference-counts">steals a reference</a> from `item` unless `keepArg` is true.

		The returned handler is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions. This is mentioned in their documentation.

		@param list handler of the list object to add an item to. It can either be obtained from a previous Python call or implied by a `PyObject *` pointer obtained from a CPython call.
		@param item handler of the item to add. It can either be obtained from a previous Python call or implied by a `PyObject *` pointer obtained from a CPython call. 
		@param keepArg if true, no reference will be stolen from `item`
		@exception BindingLogicError if Python is not initialized or if `list` is not a list

		<b>Example</b>
		@code
		const auto list(Python::list({ PyLong_FromLong(42), Python::fromAscii("42") }));

		Python::addList(list, PyFloat_FromDouble(42.0));

		// Prints the length of the list ("3")
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			{
				Python::call(Python::callable(Python::moduleBuiltins, "len"), { list })
			}
		);
		@endcode
	*/
	void addList(ObjectHandler list, ObjectHandler item, bool keepArg = false);

	/**
		Converts a string to a UTF-8 CPython string object.

		The returned handler is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions. This is mentioned in their documentation.

		@param str the string to convert
		@return a handler of the UTF-8 string object representation of `str`
		@exception BindingLogicError if Python is not initialized

		<b>Example</b>
		@code
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			{ Python::fromAscii("Text to print") }
		);
		@endcode
	*/
	ObjectHandler fromAscii(std::string const& str);

	/**
		Converts a UTF-8 CPython string object to a std::string.
		This function <a href="https://docs.python.org/3/extending/extending.html#reference-counts">steals a reference</a> from `utfStr` unless `keepArg` is true.

		@param utfStr the UTF-8 string object to convert
		@param keepArg if true, no reference will be stolen from `utfStr`
		@return the std::string representation of `utfStr`
		@exception BindingLogicError if Python is not initialized or if `utfStr` is not a UTF-8 CPython string object

		<b>Example</b>
		@code
		Python::execute("text = 'Text to display'");

		const auto text(Python::toAscii(Python::object(Python::moduleMain, "text")));

		std::cout << text << std::endl;
		@endcode
	*/
	std::string toAscii(ObjectHandler utfStr, bool keepArg = false);

	/**
		Prevents an object reference from <a href="https://docs.python.org/3/extending/extending.html#reference-counts">being stolen</a>. 
		This function increases the reference count of an object under control of Python. This prevents reference-stealing functions from disposing of a reference
		that is owned by the caller.

		@param object a handler of the object to preserve. It shoud be under control of Python.
		@return a new handler of `object` 
		@exception BindingLogicError if Python is not initialized or if `object` is not under control of Python

		<b>Example</b>
		@code
		Python::execute(Python::Program({ "def giveMeFive():", "\treturn 5" }));

		const auto math(Python::import("math"));
		const auto pow(Python::callable(math, "pow"));

		const auto objectToPreserve(Python::call(Python::callable(Python::moduleMain, "giveMeFive")));

		// displays 1  5  25
		for (int y = 0; y < 3; ++y)
		{
			// Not calling 'keepArgument(objectToPreserve)' but passing straight 'objectToPreserve' would cause a crash as it would be garbage-collected after first call
			const auto res(Python::call(pow, { Python::keepArgument(objectToPreserve), PyLong_FromLong(y) }));

			std::cout << PyFloat_AsDouble(res) << " ";
		}

		// We still own a reference on 'objectToPreserve' here. We can reuse it, get rid of it right now using Python::forgetArgument() or let Python::shutdown() dispose of it
		@endcode
	*/
	ObjectHandler const& keepArgument(ObjectHandler const& object);

	/**
		Places an CPython object under control of Python.
		This makes Python aware of this object to include it to its references count handling process.

		@param object a handler of the object to be controlled. Typical use it to have it implicitely created from a `PyObject *` pointer obtained from the CPython API 
		@return a new handler of `object` 
		@exception BindingLogicError if Python is not initialized or if `object` is already under control

		<b>Example</b>
		@code
		const auto math(Python::import("math"));
		const auto pow(Python::callable(math, "pow"));

		const auto objectX(Python::controlArgument(PyLong_FromLong(5))); // Python takes control of this object obtained from CPython

		// displays 1  5  25
		for (int y = 0; y < 3; ++y)
		{
			// Not calling 'keepArgument(objectX)' but passing straight 'objectX' would cause a crash as it would be garbage-collected after
			// first call, while only controlled objects can be passed through Python::keepArgument()
			const auto res(Python::call(pow, { Python::keepArgument(objectX), PyLong_FromLong(y) }));

			std::cout << PyFloat_AsDouble(res) << " ";
		}

		// We still own a reference on 'objectX'. We can reuse it, get rid of it right now using Python::forgetArgument() or let Python::shutdown() dispose of it
		@endcode
	*/
	ObjectHandler const& controlArgument(ObjectHandler object); // Give Python the ownership of the passed objet created from outside without changing its references count. Object should not be controlled already. 

	/**
		Get rid of a reference of an object under control.
		This function allows saving memory by decreasing the reference count of an object that is no longer used without waiting for `shutdown()` to do it.

		@param object a handler of the object to forget
		@exception BindingLogicError if Python is not initialized, if `object` is not under control, or if the references count of `object` is zero or less

		<b>Example</b>
		@code
		const auto randint(Python::callable(Python::import("random"), "randint"));
		const auto min(Python::controlArgument(PyLong_FromLong(0)));
		const auto max(Python::controlArgument(PyLong_FromLong(10)));

		// Prints 100000 random numbers between min and max included
		for (int i = 0; i < 100000; ++i)
		{
			const auto result(Python::call(randint, { Python::keepArgument(min), Python::keepArgument(max) }));

			std::cout << PyLong_AsLong(result) << std::endl;

			// We get rid of 'result' right away as we won't use it anymore.
			// Otherwise, with many iterations, a large amount of memory could end up wasted waiting for 'Python::shutdown()' to dispose of these references.
			// In a lesser extent this might also have an impact on performances as the internal references table maintain by Python would get bigger and bigger.
			Python::forgetArgument(result);
		}

		// Let's say we leave 'Python::shutdown()' to dispose of the remaining references of 'min' and 'max', that's ok.
		@endcode
	*/
	void forgetArgument(ObjectHandler const& object); // Forgets an object under control after having decreased its references count

	/**
		Enters a critical section, preventing other threads from entering any Python critical section.

		This function is relevant in multi-threaded applications, either to guarantee data consistency or to prevent concurrent calls to native or CPython functions from
		causing a crash (see note regarding	concurrency in the detailed description of this namespace).

		Calls to this function should be followed in the same thread by as many calls to `endCriticalSection()`. Not doing so is
		a cause of deadlocks.

		@exception BindingLogicError if Python is not initialized.

		<b>Example</b>
		@code
		Python::init("pythonBinding");

		const long nbIt(15000L);
		std::vector<Python::ObjectHandler> values;

		std::thread producer
		(
			[nbIt, &values]()
			{
				Python::execute(Python::Program({ "def dbl(x):", "\treturn 2 * x" }));
				const auto dbl(Python::callable(Python::moduleMain, "dbl"));

				for (long i = 0; i < nbIt; ++i)
				{
					Python::beginCriticalSection();

					const auto res(Python::call(dbl, { PyLong_FromLong(i) }));
					values.push_back(res);

					Python::endCriticalSection();
				}
			}
		);

		std::thread consumer
		{
			[nbIt, &values]()
			{
				long expected(0);

				while (expected < (2 * nbIt))
				{
					Python::beginCriticalSection();

					if (values.size())
					{
						const auto obj(*values.begin());

						// do something with 'obj'

						Python::forgetArgument(obj);
						values.erase(values.begin());

						Python::endCriticalSection();

						expected += 2;
					}
					else
					{
						Python::endCriticalSection();

						std::this_thread::sleep_for(std::chrono::milliseconds(5));
					}
				}
			}
		};

		producer.join();
		consumer.join();

		Python::shutdown();
		@endcode
	*/
	void beginCriticalSection();


	/**
		Exits a critical section, allowing other threads to enter a Python critical section.

		Calls to this function should match calls to `beginCriticalSection()` performed in the same thread.

		@exception BindingLogicError if this call doesn't match a previous call to `beginCriticalSection()` performed in the same thread.
	*/
	void endCriticalSection();
}

}
