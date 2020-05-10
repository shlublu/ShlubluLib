#pragma once

#ifdef _DEBUG
#include <cmath>
#endif


// Macro that display compile time "TODO" and "DEBUG" message. 
// Usage: PRAGMA_TODO("Do that stuff here.") et PRAGMA_DEBUG("For debug purpose only!")
#define __stringize(x)		#x  

#ifdef _WIN32
#define __makestring(M, L)	M(L)
#define __LINESTRING__		__makestring(__stringize, __LINE__)
#define PRAGMA_TOKENSTR(t)	__FILE__ "(" __LINESTRING__ ") : " t ": "

#define PRAGMA_TODO(msg)	__pragma(message (PRAGMA_TOKENSTR("TODO") msg)) 
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



