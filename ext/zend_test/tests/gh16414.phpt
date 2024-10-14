--TEST--
GH-16414 (zend_test.observer.observe_function_names may segfault)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=0
--FILE--
<?php
function bar() {}
var_dump(ini_set("zend_test.observer.observe_function_names", "bar"));
?>
--EXPECT--
bool(false)
