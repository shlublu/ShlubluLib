# ShlubluLib

ShlubluLib is a modular, general purpose, open-source C++ library for Linux and Windows.

This library consists in code I wrote for my own use and that might be useful to others. It is therefore released "as-is", just to be helpful with no 
warranty of any kind (see license for further details).

ShlubluLib is licensed under the EUPL-1.2-or-later (European Union Public Licence), please
see the files "EUPL LICENCE.txt", or read the [EUPL text online](https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12).


## Table of content

* [Modules](#modules)
* [Prerequisites](#prerequisites)
  * [Development environment](#development-environment)
  * [Libraries](#libraries)
* [Installation](#installation)
  * [Files system](#files-system)
  * [Visual Studio projects structure](#visual-studio-projects-structure)
  * [Build outputs](#build-outputs)
* [Unit tests](#unit-tests)
* [About the author](#about-the-author)
* [Acknowledgements](#acknowledgements) 


## Modules

This library currently consists in the following modules:
* **Async**: threading and asynchronous operations
  * ***MutexLock***: Recursive mutex helper based on [*std::recursive_mutex*](http://www.cplusplus.com/reference/mutex/recursive_mutex/?kw=recursive_mutex)
* **Binding**: interactions with other languages 
  * ***Python*** based on the [standard Python/C API](https://docs.python.org/3/c-api/index.html), this module is intended to make Python integration easier
* **Text**: text content handling
  * ***String***: helper functions that are not included in [*std::string*](http://www.cplusplus.com/reference/string/string/)
* **Util**: miscellaneous
  * ***Debug***: Macros useful for developing and debugging: compilation messages, optimization control, and so on
  * ***Exceptions***: Named exceptions derived from [std::exception](http://www.cplusplus.com/reference/exception/exception/) 


## Prerequisites

The version numbers cited in this section correspond to those used in development and testing.

### Development environment

This library is developed under [**Microsoft Visual Studio 2019**](https://visualstudio.microsoft.com/fr/vs/) using the following features and extensions:
* [**Microsoft Python - C++ projects debugging support**](https://visualstudio.microsoft.com/fr/vs/features/python/?wt.mc_id=aka_ms_python) extension
* Integrated [**Microsoft CppUnitTest**](https://docs.microsoft.com/en-us/visualstudio/test/microsoft-visualstudio-testtools-cppunittestframework-api-reference?view=vs-2019) framework
* Integrated [**Microsoft Linux Development**](https://devblogs.microsoft.com/cppblog/linux-development-with-c-in-visual-studio/) features

**Using Visual Studio is not an absolute prerequisite though**. The modules codebase compiles under GCC 7.3 or above. Only makefiles and unit tests are
specific to Visual Studio.

### Libraries

The following libraries are required for ShlubluLib to compile and execute:
* [**Python** v3.7](https://www.python.org/downloads/release/python-370) or above
* [**Boost** v1.67](https://www.boost.org/) or above


## Installation

There is no binary distribution so far. ShlubluLib has to be built from sources. 

### Files system 

The code is organized as follows on the files system:

	|
	|____doc/
	|	|____(module A).md
	|	|____(module B).md
	|
	|____src/
	|	|____(module A)/
	|	|	|____(feature X.h)
	|	|	|____(feature X.cpp)
	|	|	|____(feature Y.h)
	|	|	|____(feature Y.cpp)
	|	|	
	|	|____(module B)/
	|		|____(...)
	|
	|____tests/
	|	|____(module A)/
	|	|	|____(tests X.cpp)
	|	|	
	|	|____(module B)/
	|		|____(...)
	|
	|____(VS main "solution" file).sln
	|
	|____(VS test project files)
	|____(VS Linux project files)
	|____(VS Windows project files)
	|
	|____EUPL LICENSE.txt
	|____README.md


The file to open with Visual Studio is the main "solution" file ***shlublu.sln***.

### Visual Studio projects structure

The Visual Studio projects structure looks like this:

	|	
	|____doc/
	|	|____(module A).md
	|	|____(module B).md
	|
	|____EUPL LICENSE.txt
	|____README.md
	|	
	|____00tests-shlublu
	|	|____tests/
	|		|____(module A)
	|		|	|____(tests X.cpp)
	|		|	
	|		|____(module B)
	|			|____(...)
	|
	|____shlublu
	|	|____src/
	|		|____(module A)
	|		|	|____(feature X.h)
	|		|	|____(feature X.cpp)
	|		|	|____(feature Y.h)
	|		|	|____(feature Y.cpp)
	|		|	
	|		|____(module B)
	|			|____(...)
	|
	|____shlublu-linux
		|____src/
	 		|____(module A)
	 		|	|____(feature X.h)
	 		|	|____(feature X.cpp)
	 		|	|____(feature Y.h)
	 		|	|____(feature Y.cpp)
	 		|	
	 		|____(module B)
				|____(...)
	 

The projects **shlublu** (Windows) and **shlublu-linux** (Linux) can be built independantly of each other. 
**00tests-shlublu**, on its end, depends on **shlublu**.

***BEFORE YOU BUILD:*** Projects settings have to be modified according to you local environment:
* **00tests-shlublu**: 
  * ***VC++ directories***: 
    * Includes: paths to the Boost and C-Python include files of your local Windows environment
    * Libraries: path to the Boost library files of your Windows environment
* **shlublu**: 
  * ***VC++ directories***: 
    * Includes: paths to the Boost and C-Python include files of your local Windows environment
* **shlublu**: 
  * ***VC++ directories***: 
    * Includes: paths to the Boost and C-Python include files of your remote Linux environment

I am still working on making this cleaner and easier to use.

### Build outputs

* **00tests-shlublu** creates a test suite that can be used from the Test Explorer tab of Visual Studio
* **shlublu** outputs to:
  * your local Windows environment: `<\path\to>\ShlubluLib\x64\<Debug|Release>\shlublu.lib`
* **shlublu-linux** outputs to:
  * your local Windows environment: `<\path\to>\ShlubluLib\bin\x64\<Debug|Release>\libshlublu-linux.lib`
  * your remote Linux environment: `~/projects/shlublu-linux/bin/x64/<Debug|Release>\libshlublu-linux.lib`

Client projects should add the following directory to their include path:
* **Windows**: `<\path\to>\ShlubluLib\src\` 
* **Linux**: `~/projects/shlublu-linux/src`

## Unit tests

Unit tests are available for Windows only. Coverage is not complete but is improving.


## About the author

My name is Vincent Poulain ([GitHub](https://github.com/shlublu), 
[StackOverflow](https://stackoverflow.com/users/840960/shlublu), [Twitter](https://twitter.com/shlublu): @shlublu). 


## Acknowledgements

Thank you to so many Stack Overflow contriutors. And to [Stack Overflow](https://stackoverflow.com/) themselves.

