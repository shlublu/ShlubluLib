#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();

#include <shlublu/binding/Python_ObjectHandler.h>

#include<utility>

#include <shlublu/async/MutexLock.h>


static shlublu::MutexLock __pythonSequenceLock;


namespace shlublu
{

namespace Python
{

int64_t ObjectHandler::Hasher::operator () (ObjectHandler const& key) const
{
	return std::hash<int64_t>()(key.id());
}


uint64_t ObjectHandler::sSequence(0);


uint64_t ObjectHandler::nextId()
{
	__pythonSequenceLock.queueLock();

	const uint64_t ret(++sSequence);

	__pythonSequenceLock.unlock();

	return ret;
}


ObjectHandler::ObjectHandler() 
	: mPyObj(nullptr),
	  mId(0)
{}


ObjectHandler::ObjectHandler(ObjectHandler const& src)
	: mPyObj(src.mPyObj),
	  mId(src.mId)
{}


ObjectHandler::ObjectHandler(ObjectHandler&& src) noexcept
	: mPyObj(std::move(src.mPyObj)),
	  mId(std::move(src.mId))
{}


ObjectHandler::ObjectHandler(PyObject* pyObj)
	: mPyObj(pyObj),
	  mId(nextId())
{}


ObjectHandler& ObjectHandler::operator = (ObjectHandler src) noexcept
{
	swap(src);
	return *this;
}


void ObjectHandler::swap(ObjectHandler& other) noexcept
{
	if (this != &other)
	{
		std::swap(mPyObj, other.mPyObj);
		std::swap(mId, other.mId);
	}
}


PyObject* ObjectHandler::get() const
{
	return mPyObj;
}


uint64_t ObjectHandler::id() const
{
	return mId;
}


ObjectHandler::operator PyObject* () const
{
	return get();
}


bool operator == (ObjectHandler const& lhs, ObjectHandler const& rhs)
{
	return lhs.id() == rhs.id();
}


bool operator != (ObjectHandler const& lhs, ObjectHandler const& rhs)
{
	return !(lhs == rhs);
}

}

}