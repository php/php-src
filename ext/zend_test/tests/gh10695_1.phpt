--TEST--
GH-10695: Exceptions in resource dtors during shutdown are caught
--EXTENSIONS--
zend_test
--FILE--
<?php
set_exception_handler(function (\Throwable $exception) {
    echo 'Caught: ' . $exception->getMessage() . "\n";
});

$resource = zend_test_create_throwing_resource();
?>
--EXPECT--
Caught: Throwing resource destructor called
