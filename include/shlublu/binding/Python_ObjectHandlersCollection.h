#pragma once

/** @file
	Subpart of the Python module.

	See ObjectHandlersCollection documentation for details.
*/

#include <unordered_set>

#include <shlublu/binding/Python_BindingException.h>
#include <shlublu/binding/Python_ObjectHandler.h>


namespace shlublu
{

namespace Python
{

/**
	Collection of ObjectHandler used internally by Python.

	Each element of a collection instance correspond to a unique ObjectHandler use case.
*/
class ObjectHandlersCollection
{
public:
	/**
		Definition of the storage container.
	*/
	using Storage = std::unordered_set<ObjectHandler, ObjectHandler::Hasher>;

public:
	/**
		Empty constructor.
		Creates an empty collection.
	*/
	ObjectHandlersCollection();

	/**
		Copy constructor is deleted.
	*/
	ObjectHandlersCollection(ObjectHandlersCollection const&) = delete;

	/**
		Move constructor is deleted.
	*/
	ObjectHandlersCollection(ObjectHandlersCollection&&) = delete;

	/**
		Registers a use case represented by an ObjectHandler.

		@param oh the use case to register
		@return a reference to this use case once registered
		@exception BindingException if `oh`'s ID is already registered
	*/
	ObjectHandler const& registerObject(ObjectHandler const& oh);
	
	/**
		Unregisters a use case represented by an ObjectHandler.

		@param oh the use case to unregister
		@exception BindingException if `oh`'s ID has not been registered or has been unregistered already
	*/
	void unregisterObject(ObjectHandler const& oh);

	/**
		Clears the collection. 

		All use cases are unregistered by a call to this function.
	*/
	void clear();

	/**
		Returns the registration status of a use case represented by an ObjectHandler.

		@param oh the use case to assess
		@return the registration status of `oh` based on its ID
	*/
	bool isRegistered(ObjectHandler const& oh) const;

	/**
		Returns the size of this collection.

		@return the number of elements it contains.
	*/
	size_t size() const;

	/**
		Returns an iterator to the first element of this collection.
		Order is internal to ObjectHandlersCollection. It does not follow the registration order.
		Iterating from `begin()` to `end()` guarantees to go through each element once and only once.

		@return an iterator to the first element of the collection
	*/
	Storage::const_iterator begin() const;

	/**
		Returns an iterator to the past-the-end element of this collection.
		Order is internal to ObjectHandlersCollection. It does not follow the registration order.
		Iterating from `begin()` to `end()` guarantees to go through each element once and only once.

		@return an iterator to the past-the-end element of the collection
	*/
	Storage::const_iterator end() const;

private:
	/// @cond INTERNAL

	Storage mObjects;

	/// @endcond
};


}

}


