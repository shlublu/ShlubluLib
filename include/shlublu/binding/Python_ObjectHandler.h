#pragma once

/** @file
	Subpart of the Python module.

	See ObjectHandler documentation for details.
*/

#define PY_SSIZE_T_CLEAN
#include <Python.h>


namespace shlublu
{

namespace Python
{

/**
	Handler of CPython object pointers used by Python to handle references counts.
	An instance represents a specific use of a given CPython object and is identified by an ID unique to this use case. 
	
	A given CPython object can be involved in several instances should it have several use cases at a
	given point of time.
	
	Two copies of a same handler have the same object pointers and the same ID as they refer to the same use case.

	ID is never zero but for empty instances.

	Conversion from and to `PyObject *` is silent.

	@see <a href="https://docs.python.org/3/c-api/structures.html#c.PyObject">PyObject</a>

	<b>Typical examples of use</b>
	@code
	#include <iostream>

	#include <shlublu/binding/Python.h>

	using namespace shlublu;

	int main(int, char* argv[])
	{
		int retCode(0);

		try
		{
			Python::init(*argv);
			Python::execute("myFloat = 5.5");
			Python::ObjectHandler const& myFloat(Python::object(Python::moduleMain, "myFloat")); // Type 'Python::ObjectHandler const&' is explicit for this example though it is best to use `const auto`

			std::cout << PyFloat_AsDouble(myFloat) << std::endl; // 'Python::ObjectHandler' converts silently to CPython's 'PyObject *'

			const double anotherFloat(3.3);

			Python::controlArgument(PyFloat_FromDouble(anotherFloat)); // 'Python::ObjectHandler' is created silently from the CPython's 'PyObject *'

			// Python::shutdown() will dispose of this reference to the Python object created from anotherFloat
		}
		catch (Python::BindingException const& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
			retCode = -1;
		}

		return retCode;
	}
	@endcode
*/
class ObjectHandler
{
public:
	/**
		Key hasher to be used with unordered containers such as `std::unordered_map` or `std::unordered_set`.
		@see <a href="https://www.cplusplus.com/reference/unordered_map/unordered_map/">std::unordered_map</a>
		@see <a href="https://www.cplusplus.com/reference/unordered_map/unordered_qet/">std::unordered_set</a>

		<b>Typical examples of use</b>
		@code
		std::unordered_set<ObjectHandler, ObjectHandler::Hasher> objectHandlersSet;
		@endcode
	*/
	class Hasher
	{
	public:
		/**
			Returns the key hash of an instance of ObjectHandler.
			Hash is based on `ObjectHandler::id()` only.
		*/
		int64_t operator()(ObjectHandler const& key) const;
	};


public:
	/**
		Empty constructor.
		Sets both object pointer and ID to zero.
	*/
	ObjectHandler();

	/**
		Copy constructor.
		Both object pointer and ID are copied.

		@param src the instance to be copied.
	*/
	ObjectHandler(ObjectHandler const& src);

	/**
		Move constructor.
		Both object pointer and ID are moved.

		@param src the instance to move in.
	*/
	ObjectHandler(ObjectHandler&& src) noexcept;

	/**
		`PyObject*` constructor.
		Typically used as an implicit constructor for objects returned by the CPython API.
		An ID is allocated automatically at creation time.

		@param pyObj the CPython object pointer this instance will be assigned.
		@see <a href="https://docs.python.org/3/c-api/structures.html#c.PyObject">PyObject</a>
	*/
	ObjectHandler(PyObject* pyObj);

	/**
		Assignment operator.

		Should `src` be another instance, both object pointer and ID are copied.
		Should src be an object pointer, this pointer is copied and an ID is allocated automatically by an implied ObjectHandler.

		@param src other instance or `PyObject *`
		@return a reference to `*this`
	*/
	ObjectHandler& operator = (ObjectHandler src) noexcept;

	/**
		Exchanges two instances.
		Both object pointer and ID are swaped.
		Should src be an object pointer, an implied ObjectHandler is created and assigned an ID.

		@param other other instance to swap with
	*/
	void swap(ObjectHandler& other) noexcept;

	/**
		Returns the object pointer this handler encapsulates.
		@return a CPython object pointer
	*/
	PyObject* get() const;

	/**
		Returns the ID of the use case represented by this handler.
		@return a use case ID
	*/
	uint64_t id() const;

	/**
		`PyObject* ` cast operator.
		Typically used as an implicit cast operator to pass instances of ObjectHandler to functions of the CPython API.

		@return the CPython object pointer this handler encapsulates.
		@see <a href="https://docs.python.org/3/c-api/structures.html#c.PyObject">PyObject</a>
	*/
	operator PyObject* () const;

private:
	/// @cond INTERNAL
	PyObject* mPyObj;
	uint64_t mId;

private:
	uint64_t nextId();

	static uint64_t sSequence;
	/// @endcond 
};


/**
	Equality operator.
	Compare use cases represented by two instances ObjectHandler. Refers to the ID to do so as two identical ID should come with two identical object pointers while the reverse is not true.

	@param lhs left operand
	@param rhs right operand
*/
bool operator == (ObjectHandler const& lhs, ObjectHandler const& rhs);

/**
	Inequality operator.
	Compare use cases represented by two instances ObjectHandler. Refers to the ID to do so as two identical ID should come with two identical object pointers while the reverse is not true.

	@param lhs left operand
	@param rhs right operand
*/
bool operator != (ObjectHandler const& lhs, ObjectHandler const& rhs);

}

}


