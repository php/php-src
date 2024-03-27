--TEST--
GH-10695: Uncaught exception in exception handler catching resource dtor exception
--EXTENSIONS--
zend_test
--FILE--
<?php
set_exception_handler(function (\Throwable $exception) {
    throw new Exception('Caught');
});

$resource = zend_test_create_throwing_resource();
?>
--EXPECTF--
Fatal error: Uncaught Exception: Caught in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(Object(Exception))
#1 {main}
  thrown in %s on line %d
