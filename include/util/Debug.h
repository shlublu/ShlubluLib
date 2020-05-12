#pragma once

/** @file 
    Macros useful for developing and debugging: compilation messages, optimization control, and so on.
*/

/** \def PRAGMA_TODO(message)
    Displays a compile-time "TODO" message.

    <b>Example</b>
    @code
    #include <util/Debug.h>

    int main(void)
    {
        PRAGMA_TODO("Doing something useful is needed here.");

        return 0;
    }
    @endcode

    Output at compile time:
   @code
   1>Main.cpp(5) : TODO: Doing something useful is needed here.
   @endcode
*/

/** \def PRAGMA_DEBUG(message)
    Displays a compile-time "DEBUG" message.

    <b>Example</b>
    @code
    #include <string>
    #include <util/Debug.h>

    std::string const& myBogusString()
    {
        PRAGMA_DEBUG("Check this out, a bug has been reported around here.");
        return "reference to temporary, this is likely to fail";
    }
    @endcode

    Output at compile time:
    @code
    1>Feature.cpp(6) : DEBUG: Check this out, a bug has been reported around here.
    @endcode
*/

/** \def PRAGMA_OPTIMIZE_OFF()
    Prevent any compiler optimization from being performed in the current file after this statement. 
    
    This allows doing step-by-step debugging in release mode.
    This macro is only useful with Windows as its debug mode is very slow due to the numerous STL checks it performs at run-time.
    It is not implemented for Linux which has a workable debug mode: a warning is displayed instead.

    <b>Example</b>
    @code
    #include <util/Debug.h>
    PRAGMA_OPTIMIZE_OFF();

    int main(void)
    {
        // stuff

        return 0;
    }
    @endcode

    Output at compile time:
    
    Windows: @code 1>Main.cpp(2) : DEBUG: All optimizations disabled @endcode
    Linux: @code 1>Main.cpp(2,13): warning : PRAGMA_OPTIMIZE_OFF() has no effect with this platform. @endcode
*/

/// @cond INTERNAL
#define __stringize(x)		#x  
/// @endcond

#ifdef _WIN32
#define __makestring(M, L)	M(L)
#define __LINESTRING__		__makestring(__stringize, __LINE__)
#define PRAGMA_TOKENSTR(t)	__FILE__ "(" __LINESTRING__ ") : " t ": "

#define PRAGMA_TODO(msg /**< msg is there */)	__pragma(message (PRAGMA_TOKENSTR("TODO") msg)) 
#define PRAGMA_DEBUG(msg)	__pragma(message (PRAGMA_TOKENSTR("DEBUG") msg)) 

// Macros that turn optimizations on or off
#ifndef _WIN64
#define __OPTIMIZATION_LIST		"gsty"
#else
#define __OPTIMIZATION_LIST		"gst"
#endif

#define PRAGMA_OPTIMIZE_OFF()	__pragma (optimize( __OPTIMIZATION_LIST, off ))		\
								PRAGMA_DEBUG("All optimizations disabled")

#else
#define PRAGMA_TODO(msg)		_Pragma (__stringize(message "TODO: " msg))
#define PRAGMA_DEBUG(msg)		_Pragma (__stringize(message "DEBUG: " msg))

#define PRAGMA_OPTIMIZE_OFF()	_Pragma (__stringize(GCC warning "PRAGMA_OPTIMIZE_OFF() has no effect with this platform."))
#endif



