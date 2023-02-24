#include <shlublu/binding/Python_ObjectHandlersCollection.h>

#include <shlublu/text/String.h>


namespace shlublu
{

namespace Python
{

ObjectHandlersCollection::ObjectHandlersCollection()
	: mObjects()
{}


ObjectHandler const& ObjectHandlersCollection::registerObject(ObjectHandler const& oh)
{
	if (mObjects.count(oh))
	{
		throw BindingLogicError("ObjectHandlersCollection::registerObject(): object id " + String::xtos(oh.id()) + " is already registered.");
	}

	return *(mObjects.emplace(oh).first);
}
		

void ObjectHandlersCollection::unregisterObject(ObjectHandler const& oh)
{
	if (!mObjects.count(oh))
	{
		throw BindingLogicError("ObjectHandlersCollection::registerObject(): object id " + String::xtos(oh.id()) + " is not registered.");
	}

	mObjects.erase(oh);
}


void ObjectHandlersCollection::clear()
{
	mObjects.clear();
}

		
bool ObjectHandlersCollection::isRegistered(ObjectHandler const& oh) const
{
	return mObjects.count(oh) > 0;
}

		
size_t ObjectHandlersCollection::size() const
{
	return mObjects.size();
}

		
ObjectHandlersCollection::Storage::const_iterator ObjectHandlersCollection::begin() const
{
	return mObjects.begin();
}

		
ObjectHandlersCollection::Storage::const_iterator ObjectHandlersCollection::end() const
{
	return mObjects.end();
}

}

}
