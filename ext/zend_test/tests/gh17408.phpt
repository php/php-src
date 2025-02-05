--TEST--
GH-17408 (Assertion failure Zend/zend_exceptions.c)
--EXTENSIONS--
zend_test
--FILE--
<?php
function test() {
    $resource = zend_test_create_throwing_resource();
    zend_test_create_throwing_resource();
}
test();
?>
--EXPECTF--
Fatal error: Uncaught Exception: Throwing resource destructor called in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}

Next Exception: Throwing resource destructor called in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
