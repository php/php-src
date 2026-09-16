--TEST--
GH-16811 (Assertion failure adding duplicate observer handler)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_output=0
--FILE--
<?php
function foo() {}
foo();
ini_set("zend_test.observer.observe_function_names", "foo");
echo "Done\n";
?>
--EXPECT--
Done
