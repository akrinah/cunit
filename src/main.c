#include "cunit.h"

extern TestResult selftest_alltests(PrintLevel);
extern TestResult template_alltests(PrintLevel);


int main() {
  TestResult result = {};
  result = unite(result, selftest_alltests(VERBOSE));
  result = unite(result, template_alltests(SPARSE));
  result = unite(result, template_alltests(SUMMARY));
  printResult(result);
  return 0;
}
