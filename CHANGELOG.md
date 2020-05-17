## v0.3 - XXXX-XX-XX

### New features

* Python:
  * introduced `ObjectHandler`, compatible with CPython's `PyObject *`
  * added functions `beginCriticalSection()` / `endCriticalSection()`
  * added unit tests and improved the documentation.

### Fixes

* Python: fixed a multithreading issue

### Compatibility breakers

* Python: `ValueRef` and `ArgsRef` have been replaced by `ObjectHandler`. Explicit use of htese types should be replaced in your code. 

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


