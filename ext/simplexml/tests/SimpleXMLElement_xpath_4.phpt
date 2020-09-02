--TEST--
Testing xpath() with invalid XML
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml extension not loaded');
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php

try {
    simplexml_load_string("XXXXXXX^", $x, 0x6000000000000001);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $x in %s on line %d
simplexml_load_string(): Argument #3 ($options) is too large
