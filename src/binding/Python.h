#pragma once

#include <string>
#include <vector>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "../util/Exceptions.h"


namespace Python
{
	using PathEntriesList = std::vector<std::string>;
	using Instruction = std::string;
	using InstructionsList = std::vector<Instruction>;

	using ScopeRef = PyObject*;
	using CallableRef = PyObject*;
	using ValueRef = PyObject*;
	using ArgsRef = PyObject*;

	class BindingException : public ShlubluException
	{
	public:
		explicit BindingException(const std::string& message)
			: ShlubluException(("Python binding: " + message).c_str())
		{}

		explicit BindingException(const char* message)
			: BindingException(std::string(message))
		{}
	};

	extern const PathEntriesList nullPathEntriesList;
	
	extern const std::string moduleMain;
	extern const std::string moduleBuiltins;

	bool isInitialized();
	void init(std::string const& argv0, PathEntriesList const& = nullPathEntriesList);
	void shutdown();

	void execute(Instruction const& instruction);
	void execute(InstructionsList const& instructions);

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

