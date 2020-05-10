# ShlubluLib: Util module


## Debug: 

    #include <util/Debug.h>

### Macros

#### `PRAGMA_TODO(message)` and `PRAGMA_DEBUG(message)`

Displays a compile-time "TODO" or "DEBUG" message.

Example:

    #include <util/Debug.h>

    int main(void)
    {
        PRAGMA_TODO("Doing something useful is needed here");

        return 0;
    }

Output at comile time:

    1>Main.cpp(5) : TODO: Doing something useful is needed here


#### `PRAGMA_OPTIMIZE_OFF()` (Windows only)

Prevent any compiler optimization to be done in the current file after this statement. This allows doing step-by-step debugging in release mode.

This macro is only useful with Windows as its debug mode is very slow due to the numerous STL checks it performs. 
It is not even implemented for Linuw which has a workable debug mode. It displays a warning instead.

Example:

    #include <util/Debug.h>
    PRAGMA_OPTIMIZE_OFF();

    int main(void)
    {
        // stuff

        return 0;
    }

Output at compile time:

* **Windows**: `1>Main.cpp(2) : DEBUG: All optimizations disabled `
* **Linux**: `1>Main.cpp(12,13): warning : PRAGMA_OPTIMIZE_OFF() has no effect with this platform.`


## Exceptions

    #include <util/Exceptions.h>

### Classes

#### `ShlubluException: public std::exception`

All components of this library use this exception when needed.

#### `NotImplementedException: public std::exception`

Components of this library that are not implemented under a cetrain platform use this exception at run time.



