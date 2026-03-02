--TEST--
dl(): Loaded extensions support ini_set()
--SKIPIF--
<?php
include dirname(__DIR__, 3) . "/dl_test/tests/skip.inc";
if (getenv('SKIP_ASAN')) die('skip fails intermittently on ASAN');
?>
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

ini_set("dl_test.long", "1");
ini_set("dl_test.string", "world");

var_dump(ini_get("dl_test.long"));
var_dump(ini_get("dl_test.string"));

echo "OK\n";
?>
--EXPECT--
bool(true)
The extension dl_test is loaded and working!
string(12) "Hello World!"
string(1) "1"
string(5) "world"
OK
