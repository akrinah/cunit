CUnit Test API
==============

This API provides a simple way to write unit tests with as little code as necessary. It consists
of one header file and one static library.


Project Structure
-----------------

- *include/* contains all header files
- *src/* contains the sources for the library as well as the selftest and template for test suites
- *bin/* contains the executable selftest
- *lib/* contains the built library
- *temp/* contains temporary object files


Building CUnit
--------------

Execute the `make lib` command inside the project directory to compile the CUnit static library.
Use `make run` to execute a selftest and use `make clean` to delete all built files.


Adding CUnit to Your Project
----------------------------

Add the *include/cunit.h* header file to the search path of your compiler and the *lib/libcunit.a*
static library to the search path of your linker. Copy the `src/template.c` test suite template
to your project and rename it if you wish. Follow the instructions written there.


Writing a Test Suite
--------------------

Each test suite should reside in a separate *.c* file and each test case function should be
declared as `static`. This way no name collisions should occur. To call the test suite from a
`main()` function it is adviced to have an `alltests()` function inside the test suite. You can
then compile all your test suites together with a `main()` function to create an executable test.

```c
// inside template.c
#include "cunit.h"

static TestResult testCase() {
  TestResult result = {};
  apply(&result, assertEqualInt(13, 42));
  TEST(assertEqualInt(13, 42));  // does exactly the same as the line above
  return result;
}

TestResult template_alltests() {
  TestSuite suite = newSuite(__FILE__, "Test Suite Template");
  addTest(&suite, &testCase, "testCase");
  return run(&suite);
}


// inside main.c
#include "cunit.h"

extern TestResult template_alltests();

int main() {
  TestResult result = {};
  result = unite(result, template_alltests());
  printf("Result: %d of %d tests failed\n", result.failedTests, result.totalTests);
  return 0;
}
```


Writing Custom Assertions
-------------------------

CUnit comes with a bunch of assert functions.

```c
bool assertFalse(bool cond);
bool assertTrue(bool cond);

bool assertEqualBool(bool value, bool expected);
bool assertNotEqualBool(bool value, bool expected);

bool assertEqualInt(int value, int expected);
bool assertNotEqualInt(int value, int expected);

bool assertEqualChar(char value, char expected);
bool assertNotEqualChar(char value, char expected);

bool assertEqualSize(size_t value, size_t expected);
bool assertNotEqualSize(size_t value, size_t expected);

bool assertEqualFloat(float value, float expected);
bool assertNotEqualFloat(float value, float expected);

bool assertEqualDouble(double value, double expected);
bool assertNotEqualDouble(double value, double expected);

bool assertEqualString(const char* string, const char* expected);
bool assertNotEqualString(const char* string, const char* expected);

bool assertNull(const void* pointer);
bool assertNotNull(const void* pointer);

bool assertSame(const void* pointer, const void* expected);
bool assertNotSame(const void* pointer, const void* expected);

bool assertEqualMemory(const void* pointer, const void* expected, size_t length);
```

If they don't suffice your needs you can simply write your own assert function.

```c
#include "cunit.h"

#define assertEqual(val, exp) __assertEqual(__FILE__, __LINE__, val, exp)
bool __assertEqual(const char* file, int line, int value, int expected) {
  printf(__PROMPT, file, line);
  if (value == expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST "expected [%d] == [%d]\n", value, expected);
    return false;
  }
}

```


Using Macros
------------

For convinience and for better expressibility CUnit comes with a couple of macros to call your
assertions with. All you need to do is to declare a `TestResult` variable.

The `TEST()` macro is a simple wrapper around an assert function which will automatically store
the result in the `TestResult` variable.

If you want to abort a test case on a strong precondition you can use the `ABORT()` macro. It works
just like `TEST()` but exits the surrounding function if the assertion failed.

You can skip a test with the `SKIP()` macro. The `INFO()` macro can be used to print a message,
where as the `FAIL()` will print a message and add a failed test to the result.

```c
static TestResult example() {
  TestResult result = {};         // NECESSARY, macros expect it, do NOT change
  INFO("test with macros");       // simply prints a message
  TEST(assertEqualInt(13, 42));   // result += assertEqualInt(13, 42);
  SKIP(assertEqualInt(42, 42));   // result = result
  FAIL("intended to fail");       // result += false
  ABORT(assertEqualInt(42, 42));  // like TEST(assertEqualInt(42, 42)) BUT
  ABORT(assertEqualInt(13, 42));  // will exit testWithMacros() if assertion fails
  TEST(assertEqualInt(42, 42));   // won't be reached
  return result;                  // result has 3 failed tests out of 4
}

```

Data Structures and Functions
-----------------------------

The central point of CUnit is the `TestSuite` structure that stores and executes all added test
cases. Results are collected in the `TestResult` structure and propagated to the caller.

```c
typedef TestResult (*TEST_FN)();

typedef struct TestResult {
  int failedTests;
  int totalTests;
} TestResult;

typedef struct Test {
  const char* name;
  TEST_FN     fn;
} Test;

typedef struct TestSuite {
  const char* name;
  const char* description;
  int         numTests;
  Test*       tests;
} TestSuite;


TestResult unite(TestResult a, TestResult b);  // returns the united test results

void apply(TestResult* result, bool testResult);  // applies a boolean to a result

TestSuite newSuite(const char* name, const char* description);  // create new test suite

void addTest(TestSuite* suite, TEST_FN fn, const char* name);  // adds a test case to a suite

TestResult run(const TestSuite* suite);  // executes all added tests in a suite
```
