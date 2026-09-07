--TEST--
GH-16811 (Segmentation fault in zend observer)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_function_names=a,d
--FILE--
<?php
var_dump(ini_set("zend_test.observer.observe_function_names", "bar"));
function d() {}
?>
--EXPECTF--
<!-- init '%s' -->
<!-- init ini_set() -->
<!-- init var_dump() -->
string(3) "a,d"
