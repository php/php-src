--TEST--
Bug #81435 (Observer EG(current_observed_frame) may point to an old (overwritten) frame)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_function_names=a,d
opcache.optimization_level=0
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
    var_dump(new class { public function __debugInfo() { return ''; } }); // cause a bailout
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
  <!-- init var_dump() -->
  <!-- init class@anonymous::__debugInfo() -->

Fatal error: __debugInfo() must return an array in %s on line %d
</a>
