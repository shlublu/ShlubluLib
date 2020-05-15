**ShlubluLib** is a modular, general purpose, open-source C++ library for Linux and Windows.<br /> 
The current version is v0.2.

This library consists in code I wrote for my own use and that might be useful to others. It is therefore released "as-is", just to be helpful with no 
warranty of any kind (see license for further details).

ShlubluLib is licensed under the EUPL-1.2-or-later (European Union Public Licence), please
see the file "EUPL LICENCE.txt", or read the [EUPL text online](https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12).


## Table of content

* [Modules and documentation](#modules-and-documentation)
* [C++ version](#c-version)
* [Installation from binaries](#installation-from-binaries)
* [Installation from sources](#installation-from-sources)
  * [Prerequisites](#prerequisites)
    * [External libraries](#external-libraries)
    * [Development environment](#development-environment)
  * [Git repository](#git-repository)
  * [Files system](#files-system)
  * [Visual Studio projects structure](#visual-studio-projects-structure)
  * [Build outputs](#build-outputs)
* [Unit tests](#unit-tests)
* [About the author](#about-the-author)
* [Acknowledgements](#acknowledgements) 


## Modules and documentation

This library currently consists in the following modules:
* `async`: threading and asynchronous operations
  * `MutexLock`: Recursive mutex helper based on [*std::recursive_mutex*](https://www.cplusplus.com/reference/mutex/recursive_mutex/).
* `binding`: interactions with other languages
  * `Python`: based on the [CPython standard API](https://docs.python.org/3/c-api/index.html), this module is intended to make Python integration easier.
* `hash`: hash algorithms
  * `CRC`: cyclic redundancy check.
* `text`: text content handling
  * `String`: helper functions that are not included in [*std::string*](https://www.cplusplus.com/reference/string/string/).
* `util`: miscellaneous
  * `Debug`: Macros useful for developing and debugging: compilation messages, optimization control, and so on.
  * `Exceptions`: Named exceptions derived from [std::exception](https://www.cplusplus.com/reference/exception/exception/). 

The Doxygen documentation of these modules is available online at [shlublulib.shlublu.org](http://shlublulib.shlublu.org/v0.2/).

Some of these modules require external libraries such as Boost or Python. Such requirements are specified 
in their documentation.

Changes since former versions are tracked in the file `CHANGELOG.md`.


## C++ version

This is a C++17 library. 

The deprecated class template <a href="https://www.cplusplus.com/reference/locale/codecvt/">std::codecvt</a> is still used though as 
it has no standardized replacement so far.


## Installation from binaries

Should you just wish to use Shlublulib as a development tool, the binary distribution can be downloaded from our website:
* [Linux x86 version](http://shlublulib.shlublu.org/dist/shlublulib-linux-v0.2.zip) ([GPG sig](http://shlublulib.shlublu.org/dist/shlublulib-linux-v0.2.zip.asc) - SHA-256: `2b28433de4865ab0b150ecd8951c3e81a27e7dec3900ff6efd4f6eaaf95a9887`)
* [Win64 x86 version](http://shlublulib.shlublu.org/dist/shlublulib-win64-v0.2.zip) ([GPG sig](http://shlublulib.shlublu.org/dist/shlublulib-win64-v0.2.zip.asc) - SHA-256: `52d0ded07875fdc30e0369acf48eee8ba4d6ee4dae41ce957760ff7126688a3a`)

These archives contain:
* the library file to link to your client programs
* the `include` directory to add to your include path 
* the license file "EUPL LICENCE.txt" applicable to this library
* the changes log


## Installation from sources

Should you wish to contribute, to play with the code, or should you prefer not to download binaries, this method is for you.

### Prerequisites

#### External libraries

The following libraries are required for ShlubluLib to compile:
* [**Python** v0.2](https://www.python.org/downloads/release/python-370) or higher
* [**Boost** v0.2](https://www.boost.org/) or higher

Those version numbers correspond to my testing. Lower version numbers may work but this has not been tested yet.


#### Development environment

This library is developed under [**Microsoft Visual Studio 2019**](https://visualstudio.microsoft.com/fr/vs/) using the following features and extensions:
* [**Microsoft Python - C++ projects debugging support**](https://visualstudio.microsoft.com/fr/vs/features/python/?wt.mc_id=aka_ms_python) extension
* Integrated [**Microsoft CppUnitTest**](https://docs.microsoft.com/en-us/visualstudio/test/microsoft-visualstudio-testtools-cppunittestframework-api-reference?view=vs-2019) framework
* Integrated [**Microsoft Linux Development**](https://devblogs.microsoft.com/cppblog/linux-development-with-c-in-visual-studio/) remote build features
* [**Doxygen**](https://www.doxygen.nl/) documentation generator

**Visual Studio is not an absolute prerequisite**. The modules codebase compiles with GCC 5.4 or above. Only makefiles and unit tests are
specific to Visual Studio. They can be quite easily rewritten for other tools if needed. Should you plan to do that please let me know and I'll provide you with
the `g++` and `ar` command lines I use for Linux.

The same way, depending on what you would like to do, you do not need all of the above:
* **Microsoft Python - C++ projects debugging support** is optional. You can build and use Shlublulib as long as you already have a compatible Python library installed (see [External Libraries](#external-libraries) above).
* **Microsoft CppUnitTest** is only needed if you would like to build and run the unit tests suite.
* **Microsoft Linux Development for Visual Studio** is only needed if you would like to remotely build the Linux version of the library from the Visual Studio GUI.
* **Doxygen** is only needed it if you would like to generate a local version of the documentation.


### Git repository

You can either: 
* Git clone our repository (recommended option as this allows pulling updates): https://github.com/shlublu/ShlubluLib.git
* or just download a snapshot of the current version: https://github.com/shlublu/ShlubluLib/archive/master.zip


### Files system 

The repository is organized as follows on the files system:

	|
	|____doc/
	|	|____notice.txt
	|
	|____include/
	|	|____shlublu/
	|		|____(module A)/
	|		|	|____(feature X.h)
	|		|	|____(feature Y.h)
	|		|	
	|		|____(module B)/
	|			|____(...)
	|
	|____src/
	|	|____(module A)/
	|	|	|____(feature X.cpp)
	|	|	|____(feature Y.cpp)
	|	|	
	|	|____(module B)/
	|		|____(...)
	|
	|____tests/
	|	|____(module A)/
	|	|	|____(tests feature X.cpp)
	|	|	
	|	|____(module B)/
	|		|____(...)
	|
	|____ShlubluLib.sln
	|
	|____(VS unit tests subproject files)
	|____(VS Linux subproject files)
	|____(VS Windows subproject files)
	|
	|____CHANGELOG.md
	|____doxygen.conf
	|____doxygen-structure.txt
	|____EUPL LICENSE.txt
	|____README.md


The main project file to open with Visual Studio is `ShlubluLib.sln`.


### Visual Studio projects structure

The Visual Studio projects structure looks like this:

	ShlubluLib
	|	
	|____CHANGELOG.md
	|____doxygen.conf
	|____doxygen-structure.txt
	|____EUPL LICENSE.txt
	|____README.md
	|	
	|____00tests-shlublu
	|	|____tests/
	|		|____(module A)
	|		|	|____(tests X.cpp)
	|		|	|____(tests Y.cpp)
	|		|	
	|		|____(module B)
	|			|____(...)
	|
	|____shlublu
	|	|____include/
	|	|	|____(module A)
	|	|	|	|____(feature X.h)
	|	|	|	|____(feature Y.h)
	|	|	|	
	|	|	|____(module B)
	|	|		|____(...)
	|	|
	|	|____src/
	|		|____(module A)
	|		|	|____(feature X.cpp)
	|		|	|____(feature Y.cpp)
	|		|	
	|		|____(module B)
	|			|____(...)
	|
	|____shlublu-linux
		|____include/
		|	|____(module A)
		|	|	|____(feature X.h)
		|	|	|____(feature Y.h)
		|	|	
		|	|____(module B)
		|		|____(...)
		|
		|____src/
			|____(module A)
			|	|____(feature X.cpp)
			|	|____(feature Y.cpp)
			|	
			|____(module B)
				|____(...)


The projects `shlublu` (Windows) and `shlublu-linux` (Linux) can be built independantly of each other. You can 
decide to only build those you are interested in. 

The unit tests project `00tests-shlublu`, on its end, depends on `shlublu`. Building it is optional.


### Build configuration

Projects you plan to use need their settings to be modified according to your local environment:
* **`00tests-shlublu`**: 
  * ***VC++ directories***: 
    * Includes: paths to the Boost and C-Python include files of your local Windows environment
    * Libraries: path to the Boost library files of your Windows environment
  * ***Build events***: 
    * Pre-build: Doxygen generation command-line takes place here and can be removed if you prefer
* **`shlublu`**: 
  * ***VC++ directories***: 
    * Includes: paths to the Boost and C-Python include files of your local Windows environment
* **`shlublu-linux`**: 
  * ***VC++ directories***: 
    * Includes: paths to the Boost and C-Python include files of your remote Linux environment

I am working on making this cleaner and easier to use. I hope it's ok for now.


### Build outputs

* **`00tests-shlublu`** creates a test suite that can be used from the Test Explorer tab of Visual Studio
* **`shlublu`** outputs to:
  * your local Windows environment: `<\path\to\ShlubluLib>\x64\<Debug|Release>\shlublu.lib`
* **`shlublu-linux`** outputs to:
  * your local Windows environment: `<\path\to\ShlubluLib>\bin\x64\<Debug|Release>\libshlublu-linux.a`
  * your remote Linux environment: `~/projects/shlublu-linux/bin/x64/<Debug|Release>\libshlublu-linux.a`

Your client projects should also add the following directory to their include path:
* **Windows**: `<\path\to>\ShlubluLib\include\` 
* **Linux**: `~/projects/shlublu-linux/include`


## Unit tests

Unit tests are available for Windows only. Coverage is not complete but is improving.


## About the author

My name is Vincent Poulain.

You can find me there (user ID: \@shlublu):
* [GitHub](https://github.com/shlublu)
* [Stack Overflow](https://stackoverflow.com/users/840960/shlublu)
* [Twitter](https://twitter.com/shlublu)
* ![e-mail](http://shlublulib.shlublu.org/email.png)

PGP public key: [0x1412f1e42222f90d](https://keyserver.ubuntu.com/pks/lookup?search=shlublu%40yahoo.fr&fingerprint=on&op=index)

## Acknowledgements

Many thanks to:
* so many Stack Overflow contributors,
* the [Stack Overflow](https://stackoverflow.com/) team,
* all the [Boost](https://www.boost.org/) community,
* and Dimitri van Heesch for [Doxygen](https://www.doxygen.nl/)

