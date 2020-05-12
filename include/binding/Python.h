#pragma once

/** @file
	Based on the <a href="https://docs.python.org/3/c-api/index.html">CPython standard API</a>, this module is intended to make Python integration easier.
*/

#include <string>
#include <vector>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "../util/Exceptions.h"


/** @namespace Python
	Helper functions wrapping the <a href="https://docs.python.org/3/c-api/index.html">CPython standard API</a>.

	Not all functions of the CPython API are wrapped here. ShlubluLib Python is intended to make the most common operations simpler without preventing
	users to use advanced features of the official API when needed. In particular, this module focuses of making the references count handling simpler
	and less error-prone than doing it manually as CPython requires.

	@remark All functions below are thread safe. They share a MutexLock to ensure this.

	<b>Requirements for your program to compile and link:</b><br />
	Compiler include path:
	<ul>
	<li>`<path/to/your/python/distribution>/include`</li>
	</ul>
	Linker libraries, where <x> stands for the minor version of Python installed on your system:
	<ul>
	<li>Linux: `libpython3.<x>m.a`</li>
	<li>Windows: `python3<x>.dll`</li>
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
	using ValueRef = PyObject*; /**< Litterals, variables and class members. */
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
	
	extern const std::string moduleMain; /**< Main module ("__main__"). */
	extern const std::string moduleBuiltins; /**< Built-ins module ("builtins"). */

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
		@see <a href="https://www.cplusplus.com/reference/cstdlib/atexit/">::atexit()</a>
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
	// The returned values below are garbage collected at shutdown() time, though calling forget() is possible ahead.

	ScopeRef import(std::string const& moduleName);
	ScopeRef module(std::string const& moduleName);

	ValueRef variable(ScopeRef namespaceRef, std::string const& variableName);
	ValueRef variable(std::string const& moduleName, std::string const& variableName);

	CallableRef callable(ScopeRef namespaceRef, std::string const& callableName, bool forceReload = false);
	CallableRef callable(std::string const& moduleName, std::string const& callableName, bool forceReload = false);

	ArgsRef arguments(size_t size...); // Passed arguments: ValueRef's. Their references are stolen.
	ValueRef call(CallableRef callableObject, ArgsRef argumentsObject = nullptr, bool keepArguments = false); 

	//////////
	// The returned values below are NOT garbage collected as they are supposed to see their reference stolen by arguments(), tuple() or addList().
	// Call loseArgument() below manually if not the case.

	ValueRef tuple(size_t size...); // Passed arguments: ValueRef's. Their references are stolen.
	
	ValueRef list(size_t size = 0, ...); // Passed arguments: ValueRef's. Their references are stolen.
	void addList(ValueRef list, ValueRef item, bool keepArguments = false); // Passed argument: ValueRef. Its reference is stolen unless keepArguments is true.

	ValueRef fromAscii(std::string const& str);

	//////////
	// Helper functions to handle ValueRef's returned by call()
	std::string toAscii(ValueRef wstr, bool keepArgument = false); // Passed argument: ValueRef. Its reference is stolen unless keepArguments is true.

	//////////
	// Helper functions to handle references
	ValueRef keepArgument(ValueRef object); // Keep a reference on ValueRef's that are normally stolen by ref-stealing functions above and pass it under Python's control
	ValueRef controlArgument(ValueRef object); // Give Python the ownership of the passed objet created from outside without changing its references count. Object should not be controlled already. 
	void forgetArgument(PyObject* object); // Forgets an object under control after having decreased its references count
}

