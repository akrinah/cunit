#include "cunit.h"

extern TestResult selftest_alltests(PrintLevel);
extern TestResult template_alltests(PrintLevel);


int main() {
  TestResult result = {};
  result = unite(result, selftest_alltests(VERBOSE));
  result = unite(result, template_alltests(SPARSE));
  printResult(result);
  return 0;
}
