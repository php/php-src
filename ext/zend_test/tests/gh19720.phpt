--TEST--
GH-19720 (Assertion failure when error handler throws when accessing a deprecated constant)
--EXTENSIONS--
zend_test
--FILE--
<?php
class Test {
    public const MyConst = [ZEND_TEST_DEPRECATED => 42];
}

set_error_handler(function ($_, $errstr) {
    throw new Exception($errstr);
});

try {
    var_dump(Test::MyConst);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Constant ZEND_TEST_DEPRECATED is deprecated
