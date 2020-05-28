## v0.5 - 2020-05-28

### New features

* Added `Math` namespace and `math` module.
* `MutexLock`:
  * `lockLevel()` method that returns the lock level.
  * `currentThreadIsOwner()` method that returns the ownership status of the calling thread.
  * Added unit tests

### Fixes

* `MutexLock`:
  * Prevented the destructor from throwing when the mutex was locked from another thread. This is against best practices for exceptions.
* `Python`: 
  * Corrected an unaccuracte statement about multithreading in the documentation (no code was modified).
* `Random`:
  * Relaxed exception throwing criteria `min >= max` in `random()`: `min > max` is clearly sufficient.

### Compatibility breakers

* `util/Exceptions.h`:
  * Revisited the exceptions hierarchy to comply to best practices: existing exceptions from [`stdexcept`](http://www.cplusplus.com/reference/stdexcept/) are used when applicable and derived otherwise. This is only impacting for your code if it specifically catches `ShlubluException` or any of its derivations.


## v0.4 - 2020-05-23

### New features

* Added `Random` namespace and `random` module.

### Fixes

*none*

### Compatibility breakers

*none*


## v0.3 - 2020-05-19

### New features

* `MutexLock`:
  * Introduced `Guard` (`std::lock_guard<MutexLock>`)
* `Python`: 
  * revisited the whole API to make it simpler to use and more stable:
    * removed function `arguments()`. Arguments are passed straight as a vector or an initializer list to the `call()` function
    * functions `tuple()` and `list()` take a vector or an initializer list as an argument. Ellipsis are no longer used.
    * introduced `ObjectHandler`, silently compatible with CPython `PyObject *` based API 
  * added functions `beginCriticalSection()` / `endCriticalSection()` to define critical sections in multithreaded applications
  * added unit tests and improved the documentation.

### Fixes

* `Python`: fixed a multithreading issue

### Compatibility breakers

* `Python`: API has been completely revisited (see above), though its philosophy remains unchanged. Changes in your code are expected to be strightforward. 
* `MutexLock`: renamed `queueLock()` in `lock()`. Your code should be adapted the same way.


## v0.2 - 2020-05-15

### New features

* Added `CRC` class and `hash` module.

### Fixes

*none*

### Compatibility breakers

* Moved `include/<modules>` directories to `include/shlublu/<modules>`. Includes directives `#include<module/function.h>` should be replaced by `#include<shlublu/module/function.h>` in your code.
* Put all exposed symbols behind global namespace `shlublu::`. You can either prefix symbols accordingly or use `using namespace shlublu;`.
* In `util/Debug.h`: changed macros names from `PRAGMA_xxx` to `SHLUBLU_xxx` as namespaces do not apply to macros. They should be renamed in your code too.


## v0.1 - 2020-05-13

First workable version.


