#define NOMINMAX

#include "CppUnitTest.h"

#include <thread>

#include <shlublu/binding/Python.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace binding_Python
{
	TEST_CLASS(initShutdownTest)
	{
		TEST_METHOD(isInitializedWorks)
		{
			Assert::IsFalse(Python::isInitialized());

			Python::init("pythonBinding");
			Assert::IsTrue(Python::isInitialized());
			Python::shutdown();

			Assert::IsFalse(Python::isInitialized());

			Python::init("pythonBinding2");
			Assert::IsTrue(Python::isInitialized());
			Python::shutdown();

			Assert::IsFalse(Python::isInitialized());
		}


		TEST_METHOD(initRequiredToWork)
		{
			Assert::ExpectException<Python::BindingException>([]() { Python::execute("a = 1"); });

			Python::init("pythonBinding");
			Python::execute("a = 1");
			Python::shutdown();
		}


		TEST_METHOD(shutdownPreventsFromWorking)
		{
			Python::init("pythonBinding");
			Python::execute("a = 1");
			Python::shutdown();

			Assert::ExpectException<Python::BindingException>([]() { Python::execute("a = 1"); });
		}


		TEST_METHOD(shutdownResetCode)
		{
			Python::init("pythonBinding1");
			
			Python::execute("def func():\n\tx = 1\n");
			Python::call(Python::callable(Python::moduleMain, "func"));
			
			Python::shutdown();
			Python::init("pythonBinding2");

			Assert::ExpectException<Python::BindingException>([]() { Python::call(Python::callable(Python::moduleMain, "func")); });

			Python::shutdown();
		}
	};


	TEST_CLASS(executeTest)
	{
		TEST_METHOD(executeSingleProperHighLevelInstructionWorks)
		{
			Python::init("pythonBinding");
			Python::execute("a = []");

			Python::shutdown();
		}


		TEST_METHOD(executeSeriesOfProperHighLevelInstructionsWorks)
		{
			Python::init("pythonBinding");
				
			const Python::Program program = 
			{
				"a = 5",
				"b = 5",
				"c = a + b"
			};

			Python::execute(program);

			Python::shutdown();
		}


		TEST_METHOD(executeSingleWrongHighLevelInstructionThrows)
		{
			Python::init("pythonBinding");

			Assert::ExpectException<Python::BindingException>([]() { Python::execute("wrong(blah)"); });

			Python::shutdown();
		}


		TEST_METHOD(executeSeriesContainingWrongHighLevelInstructionsThrows)
		{
			Python::init("pythonBinding");

			const Python::Program program =
			{
				"a = 5",
				"wrong(blahblah)",
				"c = a + b"
			};

			Assert::ExpectException<Python::BindingException>([&program]() { Python::execute(program); });

			Python::shutdown();
		}


		TEST_METHOD(executeAfterShutdownThrows)
		{
			Python::init("pythonBinding");
			Python::shutdown();

			Assert::ExpectException<Python::BindingException>([]() { Python::execute("wrong(blah)"); });
		}


		TEST_METHOD(executeWithoutInitThrows)
		{
			Assert::ExpectException<Python::BindingException>([]() { Python::execute("wrong(blah)"); });
		}
	};


	TEST_CLASS(importTest)
	{
		TEST_METHOD(importStandardModuleWorks)
		{
			Python::init("pythonBinding");

			Assert::IsNotNull(Python::import("os"));

			Python::shutdown();
		}


		TEST_METHOD(importedModulesAreDistinct)
		{
			Python::init("pythonBinding");
			const auto importedA1(Python::import("os"));
			const auto importedA2(Python::import("os"));
			const auto importedB1(Python::import("sys"));

			Assert::AreEqual(static_cast<void*>(importedA1), static_cast<void*>(importedA2));
			Assert::AreNotEqual(static_cast<void*>(importedA1), static_cast<void*>(importedB1));

			Python::shutdown();
		}


		TEST_METHOD(importImproperModuleThrows)
		{
			Python::init("pythonBinding");

			Assert::ExpectException<Python::BindingException>([]() { Python::import("inexisting"); });

			Python::shutdown();
		}


		TEST_METHOD(importAfterShutdownThrows)
		{
			Python::init("pythonBinding");
			Python::shutdown();

			Assert::ExpectException<Python::BindingException>([]() { Python::import("os"); });
		}


		TEST_METHOD(importWithoutInitThrows)
		{
			Assert::ExpectException<Python::BindingException>([]() { Python::import("os"); });
		}
	};


	TEST_CLASS(retrieveImportTest)
	{
		TEST_METHOD(retrieveImportedModuleWorks)
		{
			const std::string moduleName("os");

			Python::init("pythonBinding");
			Python::import(moduleName);

			Assert::IsNotNull(Python::module(moduleName));

			Python::shutdown();
		}


		TEST_METHOD(retrievedImportedModulesAreRight)
		{
			const std::string moduleNameA("os");
			const std::string moduleNameB("sys");

			Python::init("pythonBinding");
				
			Python::import(moduleNameA);

			const auto retrievedA1(Python::module(moduleNameA));

			Python::import(moduleNameB);

			const auto retrievedA2(Python::module(moduleNameA));
			const auto retrievedB1(Python::module(moduleNameB));
				
			Assert::AreEqual(static_cast<void*>(retrievedA1), static_cast<void*>(retrievedA2));
			Assert::AreNotEqual(static_cast<void*>(retrievedA1), static_cast<void*>(retrievedB1));

			Python::shutdown();
		}


		TEST_METHOD(mainModuleDoesntNeedImport)
		{
			Python::init("pythonBinding");

			Assert:: IsNotNull(Python::module(Python::moduleMain));

			Python::shutdown();
		}


		TEST_METHOD(retrieveNotImportedModuleThrows)
		{
			Python::init("pythonBinding");

			Assert::ExpectException<Python::BindingException>([]() { Python::module("os"); });

			Python::shutdown();
		}


		TEST_METHOD(retrieveAfterShutdownThrows)
		{
			const std::string moduleName("os");

			Python::init("pythonBinding");
			Python::import(moduleName);
			Python::shutdown();

			Assert::ExpectException<Python::BindingException>([]() { Python::module("os"); });
		}


		TEST_METHOD(retrieveWithoutInitThrows)
		{
			Assert::ExpectException<Python::BindingException>([]() { Python::module("sys"); });
		}
	};


	TEST_CLASS(objectTest)
	{
		TEST_METHOD(objectByNamedModuleWorks)
		{
			Python::init("pythonBinding");
			
			Python::execute("testVar = 65");
			const auto var(Python::object(Python::moduleMain, "testVar"));

			Assert::IsNotNull(var);
			Assert::AreEqual(65L, PyLong_AsLong(var));

			Python::shutdown();
		}

		TEST_METHOD(objectByReferencedModuleWorks)
		{
			Python::init("pythonBinding");

			const auto moduleRef(Python::import(Python::moduleMain));

			Python::execute("testVar = 65");
			const auto var(Python::object(moduleRef, "testVar"));

			Assert::IsNotNull(var);
			Assert::AreEqual(65L, PyLong_AsLong(var));

			Python::shutdown();
		}


		TEST_METHOD(objectByReferencedObjectWorks)
		{
			Python::init("pythonBinding");

			Python::execute("class TestClass():\n\tdef __init__(self,x):\n\t\tself.xyz=x");
			const auto varA(Python::object(Python::call(Python::callable(Python::moduleMain, "TestClass"), Python::arguments(1, PyLong_FromLong(55))), "xyz"));
			const auto varB(Python::object(Python::call(Python::callable(Python::moduleMain, "TestClass"), Python::arguments(1, PyLong_FromLong(66))), "xyz"));

			Assert::IsNotNull(varA);
			Assert::AreEqual(55L, PyLong_AsLong(varA));

			Assert::IsNotNull(varB);
			Assert::AreEqual(66L, PyLong_AsLong(varB));

			Python::shutdown();
		}


		TEST_METHOD(objectInexistingThrows)
		{
			Python::init("pythonBinding");

			Assert::ExpectException<Python::BindingException>([]() { Python::callable(Python::moduleMain, "inex"); });

			Python::shutdown();
		}


		TEST_METHOD(objectAfterShutdownThrows)
		{
			const std::string moduleName("os");

			Python::init("pythonBinding");
			Python::execute("testVar = 65");

			Python::shutdown();

			Assert::ExpectException<Python::BindingException>([]() { Python::callable(Python::moduleMain, "testVar"); });
		}
	};


	TEST_CLASS(callableTest)
	{
		TEST_METHOD(callableByNamedModuleWorks)
		{
			Python::init("pythonBinding");
			Python::import("os");

			Assert::IsNotNull(Python::callable("os", "getcwd"));

			Python::shutdown();
		}

		TEST_METHOD(callableByReferencedModuleWorks)
		{
			Python::init("pythonBinding");
			const auto moduleRef(Python::import("os"));

			Assert::IsNotNull(Python::callable(moduleRef, "getcwd"));

			Python::shutdown();
		}


		TEST_METHOD(callableByReferencedObjectWorks)
		{
			Python::init("pythonBinding");

			Python::execute("class TestClass():\n\tdef __init__(self,x):\n\t\tself.xyz=x\n\n\tdef ret(self):\n\t\treturn self.xyz");
			const auto varA(Python::call(Python::callable(Python::moduleMain, "TestClass"), Python::arguments(1, PyLong_FromLong(55))));
			const auto varB(Python::call(Python::callable(Python::moduleMain, "TestClass"), Python::arguments(1, PyLong_FromLong(66))));

			Assert::IsNotNull(varA);
			Assert::AreEqual(55L, PyLong_AsLong(Python::call(Python::callable(varA, "ret"))));

			Assert::IsNotNull(varB);
			Assert::AreEqual(66L, PyLong_AsLong(Python::call(Python::callable(varB, "ret"))));

			Python::shutdown();
		}


		TEST_METHOD(callablesAreRight)
		{
			Python::init("pythonBinding");
			const auto moduleRef(Python::import("os"));

			const auto callableA1(Python::callable(moduleRef, "getcwd"));
			const auto callableB1(Python::callable(moduleRef, "getpid"));
			const auto callableA2(Python::callable(moduleRef, "getcwd"));

			Assert::AreEqual(static_cast<void*>(callableA1), static_cast<void*>(callableA2));
			Assert::AreNotEqual(static_cast<void*>(callableA1), static_cast<void*>(callableB1));

			Python::shutdown();
		}


		TEST_METHOD(callablesCanBeReloaded)
		{
			Python::init("pythonBinding");

			Python::execute("def func(x):\n\tprint('first version: ' + str(x))\n");
			const auto funcV1(Python::callable(Python::moduleMain, "func"));
			
			Python::execute("def func(x):\n\tprint('second version: ' + str(x))\n");
			const auto funcV1bis(Python::callable(Python::moduleMain, "func", false));
			const auto funcV2(Python::callable(Python::moduleMain, "func", true));

			Assert::AreEqual(static_cast<void*>(funcV1), static_cast<void*>(funcV1bis));
			Assert::AreNotEqual(static_cast<void*>(funcV1), static_cast<void*>(funcV2));

			Python::shutdown();
		}


		TEST_METHOD(callableInexistingThrows)
		{
			Python::init("pythonBinding");
			const auto moduleName("os");
			Python::import(moduleName);

			Assert::ExpectException<Python::BindingException>([&moduleName]() { Python::callable(moduleName, "inex"); });

			Python::shutdown();
		}


		TEST_METHOD(callableFromNotImportedModuleThrows)
		{
			Python::init("pythonBinding");

			Assert::ExpectException<Python::BindingException>([]() { Python::callable("os", "getcwd"); });

			Python::shutdown();
		}


		TEST_METHOD(callableAfterShutdownThrows)
		{
			const std::string moduleName("os");

			Python::init("pythonBinding");
			Python::import(moduleName);
			Python::shutdown();

			Assert::ExpectException<Python::BindingException>([&moduleName]() { Python::callable(moduleName, "getcwd"); });
		}


		TEST_METHOD(retrieveWithoutInitThrows)
		{
			Assert::ExpectException<Python::BindingException>([]() { Python::callable("os", "getcwd"); });
		}
	};


	TEST_CLASS(argumentsTest)
	{
		TEST_METHOD(argumentsWorks)
		{
			Python::init("pythonBinding");

			const auto args(Python::arguments(3, PyLong_FromLong(5), PyFloat_FromDouble(0.42), Python::fromAscii("Test")));

			Assert::IsTrue(PyTuple_CheckExact(args));

			Assert::AreEqual(Py_ssize_t(3), PyTuple_Size(args));

			Assert::IsTrue(PyLong_CheckExact(PyTuple_GetItem(args, 0)));
			Assert::IsTrue(PyFloat_CheckExact(PyTuple_GetItem(args, 1)));
			Assert::IsTrue(PyUnicode_CheckExact(PyTuple_GetItem(args, 2)));

			Assert::AreEqual(5L, PyLong_AsLong(PyTuple_GetItem(args, 0)));
			Assert::AreEqual(0.42, PyFloat_AsDouble(PyTuple_GetItem(args, 1)));
			Assert::AreEqual(std::string("Test"), Python::toAscii(PyTuple_GetItem(args, 2), true));

			Python::shutdown();
		}
	};


	TEST_CLASS(callTest)
	{
		TEST_METHOD(callWorksWithMainModule)
		{
			Python::init("pythonBinding");

			Python::execute("def sumTest(a, b):\n\treturn a + b");

			const auto resultSum
			(
				PyFloat_AsDouble
				(
					Python::call
					(
						Python::callable(Python::moduleMain, "sumTest"),
						Python::arguments(2, PyLong_FromLong(1), PyFloat_FromDouble(2.2))
					)
				)
			);

			Assert::AreEqual(3.2, resultSum);

			Python::shutdown();
		}


		TEST_METHOD(callWorksWithBuiltinsModule)
		{
			Python::init("pythonBinding");

			const auto resultStr
			(
				Python::toAscii
				(
					Python::call
					(
						Python::callable(Python::moduleBuiltins, "str"),
						Python::arguments(1, PyFloat_FromDouble(2.2))
					)
				)
			);

			Assert::AreEqual(std::string("2.2"), resultStr);

			Python::shutdown();
		}


		TEST_METHOD(callWorksWithObject)
		{
			Python::init("pythonBinding");

			Python::execute("class TestClass():\n\tdef __init__(self,x):\n\t\tself.xyz=x\n\n\tdef ret(self):\n\t\treturn self.xyz");
			const auto var(Python::call(Python::callable(Python::moduleMain, "TestClass"), Python::arguments(1, PyLong_FromLong(55))));

			Assert::AreEqual(55L, PyLong_AsLong(Python::call(Python::callable(var, "ret"))));

			Python::shutdown();
		}


		TEST_METHOD(callWorksWithCustomTuple)
		{
			Python::init("pythonBinding");

			Python::execute("def sumTest(a, b):\n\treturn a + b");

			const auto resultSum
			(
				PyFloat_AsDouble
				(
					Python::call
					(
						Python::callable(Python::moduleMain, "sumTest"),
						Python::controlArgument(Python::tuple(2, PyLong_FromLong(1), PyFloat_FromDouble(2.2)))
					)
				)
			);

			Assert::AreEqual(3.2, resultSum);

			Python::shutdown();
		}


		TEST_METHOD(callThrowsWithNonTupleArguments)
		{
			Python::init("pythonBinding");

			Assert::ExpectException<Python::BindingException>
			(
				[]()
				{
					Python::call
					(
						Python::callable(Python::moduleBuiltins, "print"),
						Python::controlArgument(Python::fromAscii("test"))
					);
				}
			);

			Python::shutdown();
		}


		TEST_METHOD(callThrowsAfterShutdown)
		{
			Python::init("pythonBinding");

			Python::execute("def sumTest(a, b):\n\treturn a + b");

			Python::shutdown();

			Assert::ExpectException<Python::BindingException>
			(
				[]() 
				{
					const auto resultSum
					(
						PyFloat_AsDouble
						(
							Python::call
							(
								Python::callable(Python::moduleMain, "sumTest"),
								Python::arguments(2, PyLong_FromLong(1), PyFloat_FromDouble(2.2))
							)
						)
					);
				}
			);
		}
	};


	TEST_CLASS(tupleTest)
	{
		TEST_METHOD(tupleWorks)
		{
			Python::init("pythonBinding");

			const auto tuple(Python::tuple(3, PyLong_FromLong(5), PyFloat_FromDouble(0.42), Python::fromAscii("Test")));

			Assert::IsTrue(PyTuple_CheckExact(tuple));

			Assert::AreEqual(Py_ssize_t(3), PyTuple_Size(tuple));

			Assert::IsTrue(PyLong_CheckExact(PyTuple_GetItem(tuple, 0)));
			Assert::IsTrue(PyFloat_CheckExact(PyTuple_GetItem(tuple, 1)));
			Assert::IsTrue(PyUnicode_CheckExact(PyTuple_GetItem(tuple, 2)));

			Assert::AreEqual(5L, PyLong_AsLong(PyTuple_GetItem(tuple, 0)));
			Assert::AreEqual(0.42, PyFloat_AsDouble(PyTuple_GetItem(tuple, 1)));
			Assert::AreEqual(std::string("Test"), Python::toAscii(PyTuple_GetItem(tuple, 2), true));

			Python::shutdown();
		}
	};


	TEST_CLASS(listTest)
	{
		TEST_METHOD(plainListWorks)
		{
			Python::init("pythonBinding");

			const auto list(Python::list(3, PyLong_FromLong(5), PyFloat_FromDouble(0.42), Python::fromAscii("Test")));

			Assert::IsTrue(PyList_CheckExact(list));

			Assert::AreEqual(Py_ssize_t(3), PyList_Size(list));

			Assert::IsTrue(PyLong_CheckExact(PyList_GetItem(list, 0)));
			Assert::IsTrue(PyFloat_CheckExact(PyList_GetItem(list, 1)));
			Assert::IsTrue(PyUnicode_CheckExact(PyList_GetItem(list, 2)));

			Assert::AreEqual(5L, PyLong_AsLong(PyList_GetItem(list, 0)));
			Assert::AreEqual(0.42, PyFloat_AsDouble(PyList_GetItem(list, 1)));
			Assert::AreEqual(std::string("Test"), Python::toAscii(PyList_GetItem(list, 2), true));

			Python::shutdown();
		}


		TEST_METHOD(addListWorks)
		{
			Python::init("pythonBinding");

			const auto list(Python::list(1, PyLong_FromLong(5)));

			Python::addList(list, PyFloat_FromDouble(0.42));
			Python::addList(list, Python::fromAscii("Test"));

			Assert::AreEqual(Py_ssize_t(3), PyList_Size(list));

			Assert::IsTrue(PyLong_CheckExact(PyList_GetItem(list, 0)));
			Assert::IsTrue(PyFloat_CheckExact(PyList_GetItem(list, 1)));
			Assert::IsTrue(PyUnicode_CheckExact(PyList_GetItem(list, 2)));

			Assert::AreEqual(5L, PyLong_AsLong(PyList_GetItem(list, 0)));
			Assert::AreEqual(0.42, PyFloat_AsDouble(PyList_GetItem(list, 1)));
			Assert::AreEqual(std::string("Test"), Python::toAscii(PyList_GetItem(list, 2), true));

			Python::shutdown();
		}
	};


	TEST_CLASS(asciiTest)
	{
		TEST_METHOD(fromAsciiWorks)
		{
			Python::init("pythonBinding");

			Python::execute("def checkEqual(x):\n\treturn x == 'testString'");

			const auto ok(Python::fromAscii("testString"));
			const auto ko(Python::fromAscii("otherString"));

			Assert::IsTrue(Python::call(Python::callable(Python::moduleMain, "checkEqual"), Python::arguments(1, ok)) == Py_True);
			Assert::IsTrue(Python::call(Python::callable(Python::moduleMain, "checkEqual"), Python::arguments(1, ko)) == Py_False);

			Python::shutdown();
		}


		TEST_METHOD(toAsciiWorks)
		{
			Python::init("pythonBinding");

			Python::execute("def returnTestString():\n\treturn 'testString'");
			const auto returned(Python::toAscii(Python::call(Python::callable(Python::moduleMain, "returnTestString"))));

			Assert::AreEqual(std::string("testString"), returned);

			Python::shutdown();
		}
	};

	TEST_CLASS(referencesCountTest)
	{
		TEST_METHOD(keepAndForgetArgumentWorks)
		{
			Python::init("pythonBinding");

			Python::execute("def returnX():\n\treturn 5");

			const auto val(Python::call(Python::callable(Python::moduleMain, "returnX")));
			const auto refCount(val->ob_refcnt);

			Python::keepArgument(val);

			Assert::IsTrue(Python::keepArgument(val) == val);
			Assert::AreEqual(refCount + 2, val->ob_refcnt);

			Python::forgetArgument(val);
			Assert::AreEqual(refCount + 1, val->ob_refcnt);

			Python::forgetArgument(val);
			Assert::AreEqual(refCount, val->ob_refcnt);

			Python::forgetArgument(val);
			Assert::AreEqual(refCount - 1, val->ob_refcnt);

			Assert::ExpectException<Python::BindingException>([&val]() { Python::forgetArgument(val); });

			Python::shutdown();
		}


		TEST_METHOD(controlAndForgetArgumentWorks)
		{
			Python::init("pythonBinding");

			const auto val(PyFloat_FromDouble(123.456));
			const auto refCount(val->ob_refcnt);

			Assert::IsTrue(Python::controlArgument(val) == val);
			Assert::AreEqual(refCount, val->ob_refcnt);

			Python::forgetArgument(val);
			Assert::AreEqual(refCount - 1, val->ob_refcnt);

			Assert::ExpectException<Python::BindingException>([&val]() { Python::forgetArgument(val); });

			Python::shutdown();
		}


		TEST_METHOD(keepArgumentThrowsIfNotUnderControl)
		{
			Python::init("pythonBinding");

			Python::execute("def returnX():\n\treturn 5");

			Assert::ExpectException<Python::BindingException>([]() { Python::keepArgument(Python::fromAscii("test")); });

			Python::shutdown();
		}

		TEST_METHOD(controlArgumentThrowsIfAlreadyUnderControl)
		{
			Python::init("pythonBinding");

			Python::execute("def returnX():\n\treturn 5");

			const auto val(Python::call(Python::callable(Python::moduleMain, "returnX")));

			Assert::ExpectException<Python::BindingException>([&val]() { Python::controlArgument(val); });

			Python::shutdown();
		}
	};

	TEST_CLASS(threadingTest)
	{
		TEST_METHOD(executeIsThreadSafe)
		{
			std::thread t1
			(
				[]()
				{
					Python::init("Thread1");
					Python::execute(Python::Program({ "sum1 = 0", "for i in range(0, 10000000):", "\tsum1 += 1" }));
				}
			);

			std::thread t2
			(
				[]()
				{
					Python::init("Thread2");
					Python::execute(Python::Program({ "sum2 = 0", "for i in range(0, 10000000):", "\tsum2 += 1" }));
				}
			);

			t1.join();
			t2.join();

			Assert::AreEqual(PyLong_AsLong(Python::object(Python::moduleMain, "sum1")), PyLong_AsLong(Python::object(Python::moduleMain, "sum2")));
			Assert::AreEqual(10000000L, PyLong_AsLong(Python::object(Python::moduleMain, "sum1")));

			Python::shutdown();
		}


		TEST_METHOD(transactionsAreThreadSafe)
		{
			Python::init("pythonBinding");

			Python::execute(Python::Program({ "def inc(x):", "\treturn x + 1" }));

			const long nbIt(5000000L);
			long x(0);
			long y(0);

			const auto job
			(
				[](long iterations) -> long
				{
					long v(0);
					const auto inc(Python::callable(Python::moduleMain, "inc"));

					for (long i = 0; i < iterations; ++i)
					{
						Python::beginCriticalSection();
						const auto pyVal(Python::call(inc, Python::arguments(1, PyLong_FromLong(v))));

						v = PyLong_AsLong(pyVal); 

						Python::forgetArgument(pyVal);
						Python::endCriticalSection();
					}

					return v;
				} 
			);
		
			std::thread tx
			(
				[&x, &job, nbIt]()
				{
					x = job(nbIt);
				}
			);
			
			y = job(nbIt);

			tx.join();

			Assert::AreEqual(x, y);
			Assert::AreEqual(nbIt, x);

			Python::shutdown();
		}
	};
}

