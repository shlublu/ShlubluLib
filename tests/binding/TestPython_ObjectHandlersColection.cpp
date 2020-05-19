#define NOMINMAX

#include "CppUnitTest.h"

#include <shlublu/binding/Python_ObjectHandlersCollection.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace binding_Python_ObjectHandlersCollection
{
    TEST_CLASS(oebjctHandlersCollectionTest)
    {
        TEST_METHOD(emptyConstructorWorks)
        {
            Python::ObjectHandlersCollection c;

            Assert::AreEqual(size_t(0), c.size());
        }


        TEST_METHOD(registerObjectAddsUnknownObjectsProperly)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            Assert::AreEqual(size_t(0), c.size());
            Assert::IsFalse(c.isRegistered(oh));

            const auto res(c.registerObject(oh));

            Assert::AreEqual(res.id(), oh.id());
            Assert::IsTrue(res.get() == oh.get());

            c.registerObject(Python::ObjectHandler(oh.get()));

            Assert::AreEqual(size_t(2), c.size());
            Assert::IsTrue(c.isRegistered(oh));
        }


        TEST_METHOD(registerObjectThrowsWhenAttemptingToAddKnownObjects)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            c.registerObject(oh);

            Assert::ExpectException<Python::BindingException>([&c, &oh]() { c.registerObject(oh); });
        }


        TEST_METHOD(unregisterObjectRemovesKnownObjectsProperly)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            c.registerObject(oh);
            c.registerObject(Python::ObjectHandler(oh.get()));

            Assert::AreEqual(size_t(2), c.size());
            Assert::IsTrue(c.isRegistered(oh));

            c.unregisterObject(oh);

            Assert::AreEqual(size_t(1), c.size());
            Assert::IsFalse(c.isRegistered(oh));
        }


        TEST_METHOD(unregisterObjectThrowsWhenAttemptingToRemoveUnknownObjects)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            c.registerObject(oh);

            Assert::ExpectException<Python::BindingException>([&c, &oh]() { c.unregisterObject(Python::ObjectHandler(oh.get())); });
        }


        TEST_METHOD(clearEmptiesEverything)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            c.registerObject(oh);
            c.registerObject(oh.get());

            Assert::AreEqual(size_t(2), c.size());

            c.clear();

            Assert::AreEqual(size_t(0), c.size());
        }


        TEST_METHOD(isRegisteredReturnsTrueForRegisteredObjects)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            Assert::IsFalse(c.isRegistered(oh));

            c.registerObject(oh);

            Assert::IsTrue(c.isRegistered(oh));
        }


        TEST_METHOD(isRegisteredReturnsFalseForNonRegisteredObjects)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            c.registerObject(oh);

            Assert::IsTrue(c.isRegistered(oh));

            c.unregisterObject(oh);

            Assert::IsFalse(c.isRegistered(oh));
        }


        TEST_METHOD(sizeCorrespondstoTheNumberOfObects)
        {
            Python::ObjectHandlersCollection c;

            const Python::ObjectHandler oh(reinterpret_cast<PyObject*>(42));

            Assert::AreEqual(size_t(0), c.size());

            c.registerObject(oh);

            Assert::AreEqual(size_t(1), c.size());

            c.registerObject(Python::ObjectHandler(oh.get()));

            Assert::AreEqual(size_t(2), c.size());
        };


        TEST_METHOD(beginsAndEndAreEqualWhenEmpty)
        {
            Python::ObjectHandlersCollection c;

            Assert::IsTrue(c.begin() == c.end());
        }
    };
}

