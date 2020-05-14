#pragma once

/** @file
	Based on the <a href="https://docs.python.org/3/c-api/index.html">CPython standard API</a>, this module is intended to make Python integration easier.

	See Python namespace documentation for details.
*/

#include <string>
#include <vector>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <shlublu/util/Exceptions.h>

namespace shlublu
{

/** @namespace Python
	Helper functions wrapping the <a href="https://docs.python.org/3/c-api/index.html">CPython standard API</a>.

	Not all functions of the CPython API are wrapped here. ShlubluLib Python is intended to make the most common operations simpler without preventing
	users to use advanced features of the official API when needed. In particular, this module focuses of making the references count handling simpler
	and less error-prone than doing it manually as CPython requires.

	@remark All functions of this namespace are thread safe. They share a MutexLock to ensure this.

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
	<li>Should you just need to execute some code with the Python Interpreter:
	@code
	#include <iostream>
	#include <vector>

	#include <binding/Python.h>
	#include <text/String.h>

	int main(int, char* argv[])
	{
		int retCode(0);

		try
		{
			const int x(5);
			const double y(2.0);

			// Initializes the Python Interpreter and give it the name of this program.
			Python::init(*argv);

			// Writes a function: sum(x,y) -> x + y, then print the result of the call of this function
			Python::execute("def sum(x, y):\n\treturn x + y");
			Python::execute("print('Sum: ' + repr(sum(" + String::xtos(x) + "," + String::xtos(y) + ")))");

			// Instructions can also be executed as a program
			const Python::Program program({ "import math", "print('Factorial: ' + repr(math.factorial(" + String::xtos(x) + ")))" });
			Python::execute(program);

			Python::shutdown(); // optional: will be called atexit() anyway
		}
		catch (Python::BindingException const& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
			retCode = -1;
		}

		return retCode;
	}
	@endcode
	</li>
	<li>Should you need your program to interact more deeply with the Python Interpreter:
	@code
	#include <iostream>
	#include <vector>

	#include <binding/Python.h>
	#include <text/String.h>

	int main(int, char* argv[])
	{
		int retCode(0);

		try
		{
			const double x(5);
			const int y(2);

			Python::init(*argv);

			// Imports math and keep a reference on it for later
			const auto math(Python::import("math"));

			// Call math.pow(y, x) using Python and some CPython functions it would have been pointless to wrap. Keep the result as a Python object
			const auto xPowY
			(
				Python::call
				(
					Python::callable(math, "pow"),                                  // takes a reference to math.pow
					Python::arguments(2, PyFloat_FromDouble(x), PyLong_FromLong(y)) // passes the arguments count, then the arguments themselves using CPython
				)
			);

			// Python::call() has decreased the reference counts of the arguments for you. You would have had to do it manually otherwise.
			// Should you need to keep them for later there are ways. This is explained later in this page.

			// Prints the result using std::iostream
			std::cout << "pow(" << x << ", " << y << "): " << PyFloat_AsDouble(xPowY) << std::endl; // displays "pow(5, 2): 25"

			// Now we compute and print the square root of this result using the interpreter (this displays "5.0")
			Python::call
			(
				Python::callable(Python::moduleBuiltins, "print"),  // takes a reference on the Python built-in print function
				Python::arguments
				(
					1,
					Python::call
					(
						Python::callable("math", "sqrt"),   // keeping a reference on imports is not mandatory. Using names is ok too as long as the import has been done.
						Python::arguments(1, xPowY)         // passes the result of the previous call as an argument
					)
				)
			);

			// Python::shutdown() will be called atexit(). This will take care of decreasing all needed references counts for you (modules, functions, arguments, results, etc).
			// You have ways to do it manually should memory be an issue. This is explained later in this page.
		}
		catch (Python::BindingException const& e)
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
*/
namespace Python
{
	using PathEntriesList = std::vector<std::string>; /**< Path as a vector of strings. */
	using RawCode = std::string; /**< Plain Python code. */
	using Program = std::vector<RawCode>; /**< Complete program. */

	using ScopeRef = PyObject*; /**< A scope is either an imported mudule, a namespace or an instance of a class. */
	using CallableRef = PyObject*; /**< Functions and methods. */
	using ValueRef = PyObject*; /**< Litterals, variables, class members or any object. */
	using ArgsRef = PyObject*; /**< Arguments to be passed to callables. */

	/**
		Should an error occur this exception will be thrown.
	*/
	class BindingException : public ShlubluException
	{
	public:
		/**
			Constructor.
			@param message description of the issue
		*/
		explicit BindingException(const std::string& message)
			: ShlubluException(("Python binding: " + message).c_str())
		{}

		/**
			Constructor.
			@param message description of the issue
		*/
		explicit BindingException(const char* message)
			: BindingException(std::string(message))
		{}
	};

	extern const Python::PathEntriesList nullPathEntriesList; /**< Empty path. */
	
	extern const std::string moduleMain; /**< Main module ("__main__"). Imported automatically by `import()`. */
	extern const std::string moduleBuiltins; /**< Built-ins module ("builtins"). Imported automatically by `import()`. */

	/**
		Check whether Python is initialized.
		Calling `shutdown()` makes Python not to be initialized anymore.
		@return the initialization status
	*/
	bool isInitialized();

	/**
		Initializes Python.
		Should `pythonSysPath` differ between two calls not separated by a `shutdown()`, the second path is appended to the first.

		This function can be called several times in a row with no issue.

		@param programName the name to give the interpreter. The `argv[0]` argument given to the `main()` function of your program is a preferred choice though not mandatory, see below
		@param pythonSysPath system path that will be appended to `sys.path` if not already part of it
		@see <a href="https://docs.python.org/3/c-api/init.html#c.Py_SetProgramName">Py_SetProgramName()</a>
	*/
	void init(std::string const& programName, PathEntriesList const& pythonSysPath = nullPathEntriesList);

	/**
		Shuts down Python.
		`shutdown()` cleans up all references to the various objects previously handled or created by using Python and calls `Py_Finalize()`.

		This function can be called several times in a row with no issue even if Python has not been initialized in the first place.
		It is called through `atexit()`, which makes its explicit use optional.

		@see <a href="https://docs.python.org/3/c-api/init.html?highlight=py_finalize#c.Py_Finalize">Py_Finalize()</a>
		@see <a href="https://www.cplusplus.com/reference/cstdlib/atexit/">atexit()</a>
	*/
	void shutdown();

	/**
		Executes raw code.

		@param code piece of code to execute. Lines should be separated by `\n`. Intentation should be materialized by spaces or `\t`.
		@exception BindingException if Python is not initialized or if the piece of code causes an error at interpretation time.
	*/
	void execute(RawCode const& code);

	/**
		Executes code as a program.
		The whole program is executed in a row and cannot be interrupted by another thread.
		
		@param program code to execute, typically splitted in lines. Intentation should be materialized by spaces or `\t`. Empty lines are permitted.
		@exception BindingException if Python is not initialized or if any line of code causes an error at interpretation time.

		<b>Example</b>
		@code
		const Python::Program program({ "def testFunc(x):", "\tprint(x + ' is now printed')", "testFunc('text to print')" });
		Python::execute(program); // prints "text to print is now printed"
		@endcode
	*/
	void execute(Program const& program);

	//////////
	// The returned values below are garbage collected automatically, though calling forgetArgument() is possible ahead.

	/**
		Imports a module by its name.
		Importing a same module several times makes the reference of the first import to be returned.

		The returned reference is under control of Python for garbage collection at `shutdown()` time only. 

		@param moduleName the name of the module, dot-delimited should it be part of a package
		@return a reference to the imported module
		@exception BindingException if Python is not initialized, if the module cannot be found or if is causes errors at interpretation time
		
		<b>Example</b>
		@code
		const auto pathModule(Python::import("os.path"));
		@endcode
	*/
	ScopeRef import(std::string const& moduleName);


	/**
		Retrieves a previously imported module.
		Modules have to be imported by `import()` to be retrieved that way. Modules imported by `execute("import <name>")` cannot
		be retrieved by this function.

		The returned reference is under control of Python for garbage collection at `shutdown()` time only.

		@param moduleName the name of the module, dot-delimited should it be part of a package
		@return a reference to the module
		@exception BindingException if Python is not initialized or if the module has not be previsouly imported by `import()` 
		
		<b>Example</b>
		@code
		Python::import("os.path")
		const auto pathModule(Python::retrieve("os.path"));
		@endcode
	*/
	ScopeRef module(std::string const& moduleName);


	/**
		Retrieves an object by its name from a scope reference.
		Objects can be retrieved regardless the way they were created.

		The returned reference is under control of Python for garbage collection. 
		Such a garbage collection is triggered by several functions that all mention this in their documentation.

		@param scopeRef reference to the module, namespace or object the object to retrieve belongs to
		@param objectName the name of the object to retrieve
		@return a reference to the retrieved object
		@exception BindingException if Python is not initialized or if the object cannot be found

		<b>Example</b>
		@code
		Python::execute(Python::Program({ "import fractions as frac", "f = frac.Fraction(3, 2)"}));

		const auto fraction(Python::object(Python::moduleMain, "f"));
		const auto denominator(Python::object(fraction, "denominator"));

		std::cout << String::xtos(PyLong_AsLong(denominator)) << std::endl;
		@endcode
	*/
	ValueRef object(ScopeRef scopeRef, std::string const& objectName);


	/**
		Retrieves an object by its name from a module name.
		Objects can be retrieved regardless the way they were created.

		The returned reference is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions that all mention this in their documentation.

		@param moduleName the name of the module the object to retrieve belongs to
		@param objectName the name of the object to retrieve
		@return a reference to the retrieved object
		@exception BindingException if Python is not initialized, if the module has not been imported by `import()`, or if the object cannot be found

		<b>Example</b>
		@code
		Python::execute(Python::Program({ "import fractions as frac", "f = frac.Fraction(3, 2)"}));

		const auto fraction(Python::object(Python::moduleMain, "f"));
		@endcode
	*/
	ValueRef object(std::string const& moduleName, std::string const& objectName);

	/**
		Retrieves a callable object (function or method) by its name from a scope reference.
		Callable objects are less likely to change than non callable objects during the execution of the program. 
		For this reason, references to callable are memorized. Multiple calls to this functions for the same scope reference and 
		callable name make the first retrieved reference to be returned, unless `forceReload` is set to true.

		The returned reference is under control of Python for garbage collection at `shutdown()` time only.

		@param scopeRef reference to the module, namespace or object the callable object to retrieve belongs to
		@param callableName the name of the callable object to retrieve
		@param forceReload if true, refresh the callable object reference before returning it and dispose of any previous version
		@return a reference to the retrieved object
		@exception BindingException if Python is not initialized, if the object cannot be found or if it is not callable

		<b>Example</b>
		@code
		const auto mathModule(Python::import("math"));
		const auto fabs(Python::callable(mathModule, "fabs"));

		std::cout << PyFloat_AsDouble(Python::call(fabs, Python::arguments(1, PyFloat_FromDouble(-42.5)))) << std::endl;
		@endcode
	*/
	CallableRef callable(ScopeRef scopeRef, std::string const& callableName, bool forceReload = false);

	/**
		Retrieves a callable object (function or method) by its name from a module name.
		Callable objects are less likely to change than non callable objects during the execution of the program.
		For this reason, references to callable are memorized. Multiple calls to this functions for the same module and
		callable names make the first retrieved reference to be returned, unless `forceReload` is set to true.

		The returned reference is under control of Python for garbage collection at `shutdown()` time only.

		@param moduleName name of the module the callable object to retrieve belongs to
		@param callableName the name of the callable object to retrieve
		@param forceReload if true, refresh the callable object reference before returning it and dispose of any previous version
		@return a reference to the retrieved object
		@exception BindingException if Python is not initialized, if the module has not been imported by `import()`, if the object cannot be found or if it is not callable

		<b>Example</b>
		@code
		Python::import("math");
		const auto fabs(Python::callable("math", "fabs"));

		std::cout << PyFloat_AsDouble(Python::call(fabs, Python::arguments(1, PyFloat_FromDouble(-42.5)))) << std::endl;
		@endcode
	*/
	CallableRef callable(std::string const& moduleName, std::string const& callableName, bool forceReload = false);

	/**
		Creates an arguments tuple to pass to `call()`.
		This function takes ownership (so decreases the references count) of the passed objects.
		Should these objects be still needed for later use, they should be passed through:
		<ul>
			<li>`keepArgument()` for objects that are under control of Python</li>
			<li>`controlArgument()` for objects that are not under control</li>
		</ul>

		The returned reference is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions that all mention this in their documentation.

		@param size the number of arguments to pass. Zero is ok to create an empty tuple.
		@param ... `size` object references. They can either be controlled by Python or directly obtained from CPython functions.
		@return a reference to the created tuple object
		@exception BindingException if Python is not initialized

		<b>Example</b>
		@code
		const auto args(Python::arguments(1, Python::fromAscii("text to print")));

		Python::call(Python::callable(Python::moduleBuiltins, "print"), args);
		@endcode
	*/
	ArgsRef arguments(size_t size...); // Passed arguments: ValueRef's. Their references are stolen.

	/**
		Calls a callable with the given arguments.

		`argumentsObject` is a tuple, typically created by `arguments()`. 
		A tuple returned by `tuple()` or by direct calls to CPython can also be used as `argumentsObject` if: 
		<ul>
			<li>control of this tuple has been given to Python using `controlArgument()`,</li>
			<li>or `keepArguments` is true</li>
		</ul>

		Once the callable returns, the references count of `argumentsObject` is decreased by a call to `forgetArgument()` unless `keepArguments` is true.

		The returned reference is under control of Python for garbage collection.
		Such a garbage collection is triggered by several functions that all mention this in their documentation.

		@param callableObject reference to the callable object to call, typically returned by `callable()`
		@param argumentsObject reference to the arguments tuple to pass, typically returned by `arguments()`. If the callable takes no argument either `nullptr` or an empty tuple is fine.
		@param keepArguments if true, the references count of `argumentsObject` will not be decreased so it can be reused later on
		@return a reference to the result of the call, that can be `None` should the callable return no value.
		@exception BindingException if Python is not initialized, if `argumentsObject` is neither `nullptr` or a tuple, or if `argumentsObject` is not under control of Python

		<b>Example</b>
		@code
		const auto print(Python::callable(Python::moduleBuiltins, "print"));
		const auto args(Python::arguments(1, Python::fromAscii("text to print")));

		Python::call(print, args);
		@endcode
	*/
	ValueRef call(CallableRef callableObject, ArgsRef argumentsObject = nullptr, bool keepArguments = false); 

	//////////
	// The returned values below are NOT garbage collected as they are supposed to see their reference stolen by arguments(), tuple() or addList().
	// Call controlArgument() below manually if not the case.

	/**
		Creates a tuple object.
		This function takes ownership (so decreases the references count) of the passed objects.
		Should these objects be still needed for later use, they should be passed through:
		<ul>
			<li>`keepArgument()` for objects that are under control of Python</li>
			<li>`controlArgument()` for objects that are not under control</li>
		</ul>

		The typical use case of this function is to prepare arguments for other functions of Python.
		So, to be consistent with the behaviour of the functions of CPython, the returned reference is NOT under control of Python for garbage collection. 
		Should a control by Python be needed or just more convenient, this reference should be passed to `controlArgument()` the same way as a
		result from a CPython function would be.

		@param size the number of elements of the tuple. Zero is ok to create an empty one.
		@param ... `size` object references. They can either be controlled by Python or directly obtained from CPython functions.
		@return a reference to the created tuple object
		@exception BindingException if Python is not initialized

		<b>Example</b>
		@code
		const auto tuple(Python::tuple(2, PyLong_FromLong(42), Python::fromAscii("42")));

		// Prints the length of the tuple ("2")
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			Python::arguments
			(
				1,
				Python::call
				(
					Python::callable(Python::moduleBuiltins, "len"),
					Python::arguments(1, tuple)
				)
			)
		);
		@endcode
	*/
	ValueRef tuple(size_t size...); 
	
	/**
		Creates a list object.
		This function takes ownership (so decreases the references count) of the passed objects.
		Should these objects be still needed for later use, they should be passed through:
		<ul>
			<li>`keepArgument()` for objects that are under control of Python</li>
			<li>`controlArgument()` for objects that are not under control</li>
		</ul>

		The typical use case of this function is to prepare arguments for other functions of Python.
		So, to be consistent with the behaviour of the functions of CPython, the returned reference is NOT under control of Python for garbage collection.
		Should a control by Python be needed or just more convenient, this reference should be passed to `controlArgument()` the same way as a
		result from a CPython function would be.

		@param size the initial number of elements of the list. Zero is ok to create it empty.
		@param ... `size` object references. They can either be controlled by Python or directly obtained from CPython functions.
		@return a reference to the created list object
		@exception BindingException if Python is not initialized

		<b>Example</b>
		@code
		const auto list(Python::list(2, PyLong_FromLong(42), Python::fromAscii("42")));

		Python::addList(list, PyFloat_FromDouble(42.0));

		// Prints the length of the list ("3")
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			Python::arguments
			(
				1,
				Python::call
				(
					Python::callable(Python::moduleBuiltins, "len"),
					Python::arguments(1, list)
				)
			)
		);
		@endcode
	*/
	ValueRef list(size_t size = 0, ...); 

	/**
		Adds an object item to the end of a list.

		This function takes ownership (so decreases the references count) of `item`.
		Should it be needed for later use, it should be passed through:
		<ul>
			<li>`keepArgument()` if it is under control of Python</li>
			<li>`controlArgument()` if it is not under control</li>
		</ul>

		@param list reference to the list object to add an item to
		@param item reference to the object itam to add. It can either be controlled by Python or directly obtained from CPython functions.
		@param keepArguments if true, the references count of `item` will not be decreased so it can be reused later on
		@exception BindingException if Python is not initialized or if `list` is not a list

		<b>Example</b>
		@code
		const auto list(Python::list(2, PyLong_FromLong(42), Python::fromAscii("42")));

		Python::addList(list, PyFloat_FromDouble(42.0));

		// Prints the length of the list ("3")
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			Python::arguments
			(
				1,
				Python::call
				(
					Python::callable(Python::moduleBuiltins, "len"),
					Python::arguments(1, list)
				)
			)
		);
		@endcode
	*/
	void addList(ValueRef list, ValueRef item, bool keepArguments = false); 

	/**
		Converts a string to a UTF-8 string object.

		The typical use case of this function is to prepare arguments for other functions of Python.
		So, to be consistent with the behaviour of the functions of CPython, the returned reference is NOT under control of Python for garbage collection.
		Should a control by Python be needed or just more convenient, this reference should be passed to `controlArgument()` the same way as a
		result from a CPython function would be.

		@param str the string to convert
		@return the UTF-8 string object representation of `str`
		@exception BindingException if Python is not initialized

		<b>Example</b>
		@code
		Python::call
		(
			Python::callable(Python::moduleBuiltins, "print"),
			Python::arguments
			(
				1,
				Python::fromAscii("Text to print")
			)
		);
		@endcode
	*/
	ValueRef fromAscii(std::string const& str);

	//////////
	// Helper functions to handle ValueRef's returned by call(), object() or straight from CPython

	/**
		Converts a UTF-8 string object to a std::string.

		The typical use case of this function is to handle string results returned by `call()` or 'object()'. 
		Such results are under control of Python. However this function can also be used with UTF-8 string objects
		obtained from functions of CPython and that are not under control of Python.

		This function takes ownership (so decreases the references count) of `wstr` unless `keepArgument` is true.

		@param wstr the UTF-8 string object to convert
		@param keepArgument if true, the references count of `wstr` will not be decreased so it can be reused later on
		@return the string representation of `wstr`
		@exception BindingException if Python is not initialized or if `wstr` is not a UTF-8 string object

		<b>Example</b>
		@code
		Python::execute("text = 'Text to display'");

		const auto text(Python::toAscii(Python::object(Python::moduleMain, "text")));

		std::cout << text << std::endl;
		@endcode
	*/
	std::string toAscii(ValueRef wstr, bool keepArgument = false); 

	//////////
	// Helper functions to handle references

	/**
		Increases the reference count of an object under control.
		This function allows keeping ownership of arguments submitted to functions that take ownership by decreasing their references count.

		@param object the object to protect from ownsership loss
		@return `object` itself
		@exception BindingException if Python is not initialized or if `object` is not under control

		<b>Example</b>
		@code
		Python::execute(Python::Program({ "def giveMeFive():", "\treturn 5" }));

		const auto math(Python::import("math"));
		const auto pow(Python::callable(math, "pow"));

		const auto pyX(Python::call(Python::callable(Python::moduleMain, "giveMeFive")));

		// displays 1  5  25
		for (int y = 0; y < 3; ++y)
		{
			// Not calling keepArgument(pyX) but passing straight pyX would cause a crash as pyX would be garbage collected after first call
			const auto res(Python::call(pow, Python::arguments(2, Python::keepArgument(pyX), PyLong_FromLong(y)))); 

			std::cout << PyFloat_AsDouble(res) << " ";
		}

		// We still have a reference on pyX. We can get rid of it right now using Python::forgetArgument() or let Python::shutdown() dispose of it
		@endcode
	*/
	ValueRef keepArgument(ValueRef object); // Keep a reference on ValueRef's that are normally stolen by ref-stealing functions above and pass it under Python's control


	/**
		Gives Python control of an object.
		This makes Python aware of this object to handle its garbage collection process.

		@param object the object to be controlled
		@return `object` itself
		@exception BindingException if Python is not initialized or if `object` is already under control

		<b>Example</b>
		@code
		const auto math(Python::import("math"));
		const auto pow(Python::callable(math, "pow"));

		const auto pyX(Python::controlArgument(PyLong_FromLong(5))); // Python controls this object obtained from CPython

		// displays 1  5  25
		for (int y = 0; y < 3; ++y)
		{
			// Not calling keepArgument(pyX) but passing straight pyX would cause a crash as pyX would be garbage collected after 
			// first call, and only controlled objects can be passed through Python::keepArgument()
			const auto res(Python::call(pow, Python::arguments(2, Python::keepArgument(pyX), PyLong_FromLong(y))));

			std::cout << PyFloat_AsDouble(res) << " ";
		}

		// We still have a reference on pyX. As it is under control we can let Python::shutdown() dispose of it
		@endcode
	*/
	ValueRef controlArgument(ValueRef object); // Give Python the ownership of the passed objet created from outside without changing its references count. Object should not be controlled already. 

	/**
		Get rid of a reference of an object under control.
		This function allows saving memory by decreasing the reference count of an object that is no longer used without waiting for `shutdown()` to do it.

		@param object the object to forget
		@exception BindingException if Python is not initialized, if `object` is not under control, or if the references count of `object` is zero

		<b>Example</b>
		@code
		const auto randint(Python::callable(Python::import("random"), "randint"));
		const auto min(Python::controlArgument(PyLong_FromLong(0)));
		const auto max(Python::controlArgument(PyLong_FromLong(10)));

		// Prints 100000 random numbers between min and max included
		for (int i = 0; i < 100000; ++i)
		{
			const auto result(Python::call(randint, Python::arguments(2, Python::keepArgument(min), Python::keepArgument(max))));

			std::cout << PyLong_AsLong(result) << std::endl;

			// We get rid of result right away as we won't use it anymore.
			// Otherwise, with many iterations, a large amount of memory could end up wasted waiting for Python::shutdown().
			// This can also have an impact on performances as the internal references table maintain by Python gets bigger.
			Python::forgetArgument(result);
		}

		// We let Python::shutdown() dispose of the remaining references of min and max, that's ok.
		@endcode
	*/
	void forgetArgument(PyObject* object); // Forgets an object under control after having decreased its references count
}

}