#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>


namespace shlublu
{

namespace Python
{

class ObjectHandler
{
public:
	class Hasher
	{
	public:
		int64_t operator()(ObjectHandler const& key) const;
	};


public:
	ObjectHandler();
	ObjectHandler(ObjectHandler const& src);
	ObjectHandler(ObjectHandler&& src) noexcept;
	ObjectHandler(PyObject* pyObj);

	ObjectHandler& operator = (ObjectHandler src) noexcept;

	void swap(ObjectHandler& other) noexcept;

	PyObject* get() const;
	uint64_t id() const;

	operator PyObject* () const;

private:
	PyObject* mPyObj;
	uint64_t mId;

private:
	uint64_t nextId();

	static uint64_t sSequence;
};


bool operator == (ObjectHandler const& lhs, ObjectHandler const& rhs);
bool operator != (ObjectHandler const& lhs, ObjectHandler const& rhs);

}

}


