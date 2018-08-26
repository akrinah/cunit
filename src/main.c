#include "cunit.h"

#include <stdio.h>


extern TestResult selftest_alltests();
extern TestResult template_alltests();


int main() {
  TestResult result = {};
  result = unite(result, selftest_alltests());
  result = unite(result, template_alltests());
  printf("Result: %d of %d tests failed\n", result.failedTests, result.totalTests);
  return 0;
}
