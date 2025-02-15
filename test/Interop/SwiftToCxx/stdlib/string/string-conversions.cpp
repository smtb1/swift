// RUN: %empty-directory(%t)
// RUN: split-file %s %t

// RUN: %target-swift-frontend -typecheck %t/print-string.swift -typecheck -module-name Stringer -enable-experimental-cxx-interop -emit-clang-header-path %t/Stringer.h

// RUN: %target-interop-build-clangxx -std=gnu++20 -c %t/string-conversions.cpp -I %t -o %t/swift-stdlib-execution.o
// RUN: %target-build-swift %t/print-string.swift -o %t/swift-stdlib-execution -Xlinker %t/swift-stdlib-execution.o -module-name Stringer -Xfrontend -entry-point-function-name -Xfrontend swiftMain %target-cxx-lib
// RUN: %target-codesign %t/swift-stdlib-execution
// RUN: %target-run %t/swift-stdlib-execution | %FileCheck %s

// REQUIRES: executable_test

//--- print-string.swift

@_expose(Cxx)
public func printString(_ s: String) {
    print("'''\(s)'''")
}

//--- string-conversions.cpp

#include <cassert>
#include "Stringer.h"

int main() {
  using namespace Swift;
  using namespace Stringer;

  {
    auto s = String("hello world");
    printString(s);
  }
// CHECK: '''hello world'''

  {
    std::string str = "test std::string";
    printString(str);
  }
// CHECK-NEXT: '''test std::string'''
  return 0;
}
