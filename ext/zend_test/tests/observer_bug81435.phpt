--TEST--
Bug #81435 (Observer EG(current_observed_frame) may point to an old (overwritten) frame)
--EXTENSIONS--
zend_test
--INI--
memory_limit=20M
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_function_names=a,d
opcache.optimization_level=0
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip requires zmm");
?>
--FILE--
<?php

function d() {} // observed

function c() {
    d();
}

function b() {
    c();
}

function bailout(...$args) {
    array_map("str_repeat", ["\xFF"], [100000000]);
}

function a() { // observed (first_observed_frame)
    b();
    bailout(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15); // overwrite the vm_stack containing prev_execute_data
}

a();

?>
--EXPECTF--
<!-- init '%s' -->
<!-- init a() -->
<a>
  <!-- init b() -->
  <!-- init c() -->
  <!-- init d() -->
  <d>
  </d>
  <!-- init bailout() -->
  <!-- init array_map() -->
  <!-- init str_repeat() -->
  <!-- Exception: MemoryError -->
</a>
<!-- init Error::__toString() -->
<!-- init Error::getTraceAsString() -->

Fatal error: Uncaught MemoryError: The resulting string is too large to fit in the configured memory limit in %s:%d
Stack trace:
#0 [internal function]: str_repeat('\xFF', 100000000)
#1 %s(%d): array_map('str_repeat', Array, Array)
#2 %s(%d): bailout(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
#3 %s(%d): a()
#4 {main}
  thrown in %s on line %d
