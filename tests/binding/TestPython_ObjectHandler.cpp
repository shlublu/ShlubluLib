#define NOMINMAX

#include "CppUnitTest.h"

#include <set>

#include <shlublu/binding/Python_ObjectHandler.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace binding_Python_ObjectHandler
{
    TEST_CLASS(oebjctHandlerHasherTest)
    {
        TEST_METHOD(operatorHashIsCorrect)
        {
            Python::ObjectHandler::Hasher hasher;

            std::set<size_t> distinct;

            const auto a(Python::ObjectHandler(reinterpret_cast<PyObject*>(42)));
            const auto b(Python::ObjectHandler(reinterpret_cast<PyObject*>(24)));

            distinct.insert(hasher(a));
            distinct.insert(hasher(b));

            distinct.insert(hasher(Python::ObjectHandler(a)));
            distinct.insert(hasher(Python::ObjectHandler(b)));

            Assert::AreEqual(size_t(2), distinct.size());
        }
    };


	TEST_CLASS(oebjctHandlerTest)
	{
        TEST_METHOD(emptyConstructorIsCorrect)
        {
            const Python::ObjectHandler oh;

            Assert::IsNull(oh.get());
            Assert::AreEqual(0ULL, oh.id());
        }


        TEST_METHOD(copyConstructorIsCorrect)
        {
            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));
            const Python::ObjectHandler copy(oh);

            Assert::AreEqual(reinterpret_cast<void*>(oh.get()), reinterpret_cast<void*>(copy.get()));
            Assert::AreEqual(oh.id(), copy.id());
        }


        TEST_METHOD(moveConstructorIsCorrect)
        {
            Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));
            const Python::ObjectHandler target(std::move(oh));

            Assert::AreEqual(reinterpret_cast<void*>(42), reinterpret_cast<void*>(target.get()));
        }


        TEST_METHOD(valueConstructorIsCorrect)
        {
            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            Assert::AreEqual(reinterpret_cast<void*>(42), reinterpret_cast<void*>(oh.get()));
        }


        TEST_METHOD(valueConstructorIsSequential)
        {
            uint64_t lastId(0);

            for (int i = 0; i < 100; ++i)
            {
                const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

                Assert::IsTrue(oh.id() > lastId);
                
                lastId = oh.id();
            }
        }


        TEST_METHOD(operatorEqualIsCorrect)
        {
            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));
            Python::ObjectHandler target;

            target = oh;

            Assert::AreEqual(reinterpret_cast<void*>(oh.get()), reinterpret_cast<void*>(target.get()));
            Assert::AreEqual(oh.id(), target.id());
        }


        TEST_METHOD(swapIsCorrect)
        {
            const auto va(reinterpret_cast<PyObject*>(42));
            const auto vb(reinterpret_cast<PyObject*>(24));

            Python::ObjectHandler a(va);
            Python::ObjectHandler b(vb);

            const auto ia(a.id());
            const auto ib(b.id());

            a.swap(b);

            Assert::AreEqual(reinterpret_cast<void*>(vb), reinterpret_cast<void*>(a.get()));
            Assert::AreEqual(reinterpret_cast<void*>(va), reinterpret_cast<void*>(b.get()));

            Assert::AreEqual(ib, a.id());
            Assert::AreEqual(ia, b.id());
        }


        TEST_METHOD(getIsCorrect)
        {
            const auto v(reinterpret_cast<PyObject*>(42));

            const Python::ObjectHandler oh(v);

            Assert::AreEqual(reinterpret_cast<void*>(v), reinterpret_cast<void*>(oh.get()));
        }


        TEST_METHOD(castIsCorrect)
        {
            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));
            PyObject* const v = oh;

            Assert::AreEqual(reinterpret_cast<void*>(v), reinterpret_cast<void*>(oh.get()));
        }


        TEST_METHOD(operatorIsEqualIsCorrect)
        {            
            const Python::ObjectHandler a(reinterpret_cast<PyObject*>(42));
            const Python::ObjectHandler b(a);
            const Python::ObjectHandler c(reinterpret_cast<PyObject*>(42));

            Assert::IsTrue(a == a);
            Assert::IsTrue(a == b);
            Assert::IsFalse(a == c);
        }


        TEST_METHOD(operatorIsNotEqualIsCorrect)
        {
            const Python::ObjectHandler a(reinterpret_cast<PyObject*>(42));
            const Python::ObjectHandler b(a);
            const Python::ObjectHandler c(reinterpret_cast<PyObject*>(42));

            Assert::IsFalse(a != a);
            Assert::IsFalse(a != b);
            Assert::IsTrue(a != c);
        }
    };
}

