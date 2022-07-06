--TEST--
Bug #81435 (Observer current_observed_frame may point to an old (overwritten) frame)
--INI--
memory_limit=20M
zend_test.observer.enabled=1
zend_test.observer.observe_function_names=a,d
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

Fatal error: Allowed memory size of 20971520 bytes exhausted %s in %s on line %d
</a>
