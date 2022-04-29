--TEST--
dl(): Extensions compiled against zend_register_ini_entries() are supported
--ENV--
PHP_DL_TEST_USE_OLD_REGISTER_INI_ENTRIES=1
--SKIPIF--
<?php include dirname(__DIR__, 3) . "/dl_test/tests/skip.inc"; ?>
--FILE--
<?php

if (extension_loaded('dl_test')) {
    exit('Error: dl_test is already loaded');
}
if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
    $loaded = dl('php_dl_test.dll');
} else {
    $loaded = dl('dl_test.so');
}

var_dump($loaded);

dl_test_test1();
var_dump(dl_test_test2("World!"));

var_dump(ini_get("dl_test.long"));
var_dump(ini_get("dl_test.string"));

echo "OK\n";
--EXPECT--
bool(true)
The extension dl_test is loaded and working!
string(12) "Hello World!"
string(1) "0"
string(5) "hello"
OK
