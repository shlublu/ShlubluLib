#include <shlublu/random/Random.h>

#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace shlublu
{

namespace Random
{

Engine* __sRandomEngine = nullptr;


static void __randomEngineCleanup()
{
	if (Random::__sRandomEngine)
	{
		delete __sRandomEngine;
		Random::__sRandomEngine = nullptr;
	}
}


static bool __initRandom()
{
	__randomEngineCleanup();

	std::random_device rd;
	__sRandomEngine = new Engine(rd());

	std::atexit(__randomEngineCleanup);

	return true;
}


static bool __RandomInitialized(__initRandom());


bool tossACoin()
{
	return probability(0.5);
}

}

}
