--TEST--
Bug GH-8466: ini_get() is optimized out when the option does not exist during compilation
--SKIPIF--
<?php include dirname(__DIR__, 2) . "/dl_test/tests/skip.inc"; ?>
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

var_dump(ini_get('dl_test.long'));
--EXPECT--
bool(true)
string(1) "0"
