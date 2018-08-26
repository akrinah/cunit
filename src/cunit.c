#include "cunit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>


/******************************************* INTERNAL ********************************************/


#define FPRECISION  0.00001f
#define DPRECISION  0.00000001


const char* tostringBool(bool b) {
  return b ? "true" : "false";
}


const char* tostringChar(char c) {
  static const char* table[] = {
      "\\0", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
      "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "SO",  "SI",
      "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
      "CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US",
      " ",   "!",   "\"",  "#",   "$",   "%",   "&",   "'",
      "(",   ")",   "*",   "+",   "´",   "-",   ".",   "/",
      "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",
      "8",   "9",   ":",   ";",   "<",   "=",   ">",   "?",
      "@",   "A",   "B",   "C",   "D",   "E",   "F",   "G",
      "H",   "I",   "J",   "K",   "L",   "M",   "N",   "O",
      "P",   "Q",   "R",   "S",   "T",   "U",   "V",   "W",
      "X",   "Y",   "Z",   "[",   "\\",  "]",   "^",   "_",
      "`",   "a",   "b",   "c",   "d",   "e",   "f",   "g",
      "h",   "i",   "j",   "k",   "l",   "m",   "n",   "o",
      "p",   "q",   "r",   "s",   "t",   "u",   "v",   "w",
      "x",   "y",   "z",   "{",   "|",   "}",   "~",   "DEL",
  };
  return table[(unsigned char) c];
}


/**
 * Compares floats and returns true if their relative distance is within some limit.
 * Special cases: -0.0f==0.0f, nan!=nan.
 * Source: https://www.floating-point-gui.de/errors/comparison/
 */
bool cmpFloat(float a, float b) {
  float absA = a < 0.0f ? -a : a;
  float absB = b < 0.0f ? -b : b;
  float diff = fabs(a - b);

  if (a == b) {
    return true;  // shortcut, covers infinities and -0.0f
  } else if (a == 0.0f || b == 0.0f || diff < FLT_MIN) {
    return diff < (FPRECISION * FLT_MIN);  // a or b close to zero, relative error thus meaningless
  } else {
    return (diff / fmin(absA+absB, FLT_MAX)) < FPRECISION;  // return relative error
  }
}

bool cmpDouble(double a, double b) {
  double absA = a < 0.0 ? -a : a;
  double absB = b < 0.0 ? -b : b;
  double diff = fabs(a - b);

  if (a == b) {
    return true;  // shortcut, covers infinities and -0.0
  } else if (a == 0.0 || b == 0.0 || diff < DBL_MIN) {
    return diff < (DPRECISION * DBL_MIN);  // a or b close to zero, relative error thus meaningless
  } else {
    return (diff / fmin(absA+absB, DBL_MAX)) < DPRECISION;  // return relative error
  }
}


/**
 * Returns -1 if strings are equal otherwise the index at which they differ.
 */
int cmpString(const char* a, const char* b) {
  int index = -1;
  bool equal = true;

  while (equal) {
    equal = *a == *b;
    index++;
    if (*a == '\0' || *b == '\0') break;
    a++; b++;
  }

  return equal ? -1 : index;
}


/**
  * Returns -1 if the memory blocks are equal otherwise the index at which they differ.
  */
int cmpMemory(const void* a, const void* b, size_t length) {
  int index;
  bool equal = true;

  for (index = 0; equal && index < length; index++) {
    equal = ((unsigned char*) a)[index] == ((unsigned char*) b)[index];
  }
  index--;  // compensate the increment on loop exit

  return equal ? -1 : index;
}


/******************************************** STRUCTS ********************************************/


TestResult unite(TestResult a, TestResult b) {
  TestResult result = {};
  result.failedTests = a.failedTests + b.failedTests;
  result.totalTests = a.totalTests + b.totalTests;
  return result;
}


void apply(TestResult* result, bool testResult) {
  result->failedTests += testResult ? 0 : 1;
  result->totalTests += 1;
}


TestSuite newSuite(const char* name, const char* description) {
  TestSuite suite = {};
  suite.name = name;
  suite.description = description;
  return suite;
}


void addTest(TestSuite* suite, TEST_FN fn, const char* name) {
  Test* temp = suite->tests;
  suite->tests = (Test*) malloc((suite->numTests + 1) * sizeof(Test));
  suite->tests = memcpy(suite->tests, temp, suite->numTests * sizeof(Test));
  suite->tests[suite->numTests++] = (Test) { name, fn };
  free(temp);
}


TestResult run(const TestSuite* suite) {
  TestResult result = {};
  printf(BLU "[%s]" RST " %s\n", suite->name, suite->description);

  for (int i = 0; i < suite->numTests; i++) {
    Test test = suite->tests[i];
    printf("%s ...\n", test.name);
    TestResult r = test.fn();
    result.failedTests += r.failedTests;
    result.totalTests += r.totalTests;
  }

  float passedRatio = result.failedTests / (result.totalTests + FPRECISION);
  passedRatio = 100.0f * (1.0f - passedRatio);
  printf(BLU "[%s]" RST " %d of %d tests failed (%.2f%% passed)\n",
         suite->name, result.failedTests, result.totalTests, passedRatio);
  printf("\n");
  return result;
}


/****************************************** ASSERTIONS *******************************************/


bool __assertFalse(const char* file, int line, bool cond) {
  printf(__PROMPT, file, line);
  if (cond == false) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%s]\n", tostringBool(false));
    return false;
  }
}


bool __assertTrue(const char* file, int line, bool cond) {
  printf(__PROMPT, file, line);
  if (cond == true) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%s]\n", tostringBool(true));
    return false;
  }
}


bool __assertEqualBool(const char* file, int line, bool value, bool expected) {
  printf(__PROMPT, file, line);
  if (value == expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%s] == [%s]\n", tostringBool(value), tostringBool(expected));
    return false;
  }
}


bool __assertNotEqualBool(const char* file, int line, bool value, bool expected) {
  printf(__PROMPT, file, line);
  if (value != expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%s] != [%s]\n", tostringBool(value), tostringBool(expected));
    return false;
  }
}


bool __assertEqualInt(const char* file, int line, int value, int expected) {
  printf(__PROMPT, file, line);
  if (value == expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%d] == [%d]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualInt(const char* file, int line, int value, int expected) {
  printf(__PROMPT, file, line);
  if (value != expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%d] != [%d]\n", value, expected);
    return false;
  }
}


bool __assertEqualChar(const char* file, int line, char value, char expected) {
  printf(__PROMPT, file, line);
  if (value == expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%s] == [%s]\n", tostringChar(value), tostringChar(expected));
    return false;
  }
}


bool __assertNotEqualChar(const char* file, int line, char value, char expected) {
  printf(__PROMPT, file, line);
  if (value != expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%s] != [%s]\n", tostringChar(value), tostringChar(expected));
    return false;
  }
}


bool __assertEqualSize(const char* file, int line, size_t value, size_t expected) {
  printf(__PROMPT, file, line);
  if (value == expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%lu] == [%lu]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualSize(const char* file, int line, size_t value, size_t expected) {
  printf(__PROMPT, file, line);
  if (value != expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%lu] != [%lu]\n", value, expected);
    return false;
  }
}


bool __assertEqualFloat(const char* file, int line, float value, float expected) {
  printf(__PROMPT, file, line);
  if (cmpFloat(value, expected)) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%e] == [%e]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualFloat(const char* file, int line, float value, float expected) {
  printf(__PROMPT, file, line);
  if (!cmpFloat(value, expected)) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%e] != [%e]\n", value, expected);
    return false;
  }
}


bool __assertEqualDouble(const char* file, int line, double value, double expected) {
  printf(__PROMPT, file, line);
  if (cmpDouble(value, expected)) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%e] == [%e]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualDouble(const char* file, int line, double value, double expected) {
  printf(__PROMPT, file, line);
  if (!cmpDouble(value, expected)) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%e] != [%e]\n", value, expected);
    return false;
  }
}


bool __assertEqualString(const char* file, int line, const char* string, const char* expected) {
  printf(__PROMPT, file, line);
  int index = cmpString(string, expected);
  if (index == -1) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("in \"%.5s\"%s[%d] expected [%s] == [%s]\n",
           string, (strlen(string) > 5 ? "~" : ""), index,
           tostringChar(string[index]), tostringChar(expected[index]));
    return false;
  }
}


bool __assertNotEqualString(const char* file, int line, const char* string, const char* expected) {
  printf(__PROMPT, file, line);
  int index = cmpString(string, expected);
  if (index != -1) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [\"%.5s\"%s] != [\"%.5s\"%s]\n",
           string, (strlen(string) > 5 ? "~" : ""), expected, (strlen(expected) > 5 ? "~" : ""));
    return false;
  }
}


bool __assertNull(const char* file, int line, const void* pointer) {
  printf(__PROMPT, file, line);
  if (pointer == NULL) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%p] == [%p]\n", pointer, NULL);
    return false;
  }
}


bool __assertNotNull(const char* file, int line, const void* pointer) {
  printf(__PROMPT, file, line);
  if (pointer != NULL) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%p] != [%p]\n", pointer, NULL);
    return false;
  }
}


bool __assertSame(const char* file, int line, const void* pointer, const void* expected) {
  printf(__PROMPT, file, line);
  if (pointer == expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%p] == [%p]\n", pointer, expected);
    return false;
  }
}


bool __assertNotSame(const char* file, int line, const void* pointer, const void* expected) {
  printf(__PROMPT, file, line);
  if (pointer != expected) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    printf(RED "ERROR: " RST);
    printf("expected [%p] != [%p]\n", pointer, expected);
    return false;
  }
}


bool __assertEqualMemory(const char* file, int line,
                         const void* pointer, const void* expected, size_t length) {
  printf(__PROMPT, file, line);
  int index = cmpMemory(pointer, expected, length);
  if (index == -1) {
    printf(GRN "OK\n" RST);
    return true;
  } else {
    const unsigned char* p = (const unsigned char*) pointer;
    const unsigned char* e = (const unsigned char*) expected;
    printf(RED "ERROR: " RST);
    printf("at (%p)[%d] expected [0x%02x] == [0x%02x]\n", pointer, index, p[index], e[index]);
    return false;
  }
}