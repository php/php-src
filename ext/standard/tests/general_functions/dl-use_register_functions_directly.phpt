--TEST--
dl(): use zend_register_functions() directly
--ENV--
PHP_DL_TEST_USE_REGISTER_FUNCTIONS_DIRECTLY=1
--SKIPIF--
<?php require dirname(__DIR__, 3) . "/dl_test/tests/skip.inc"; ?>
--FILE--
<?php

if (extension_loaded('dl_test')) {
    exit('Error: dl_test is already loaded');
}

if (PHP_OS_FAMILY === 'Windows') {
    $loaded = dl('php_dl_test.dll');
} else {
    $loaded = dl('dl_test.so');
}

var_dump($loaded);
var_dump(dl_test_use_register_functions_directly());
?>
--EXPECT--
bool(true)
string(2) "OK"
