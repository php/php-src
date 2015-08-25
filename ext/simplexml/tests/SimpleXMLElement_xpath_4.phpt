--TEST--
Testing xpath() with invalid XML
--SKIPIF--
<?php
if (PHP_MAJOR_VERSION < 7) die("skip this test is for PHP 7+ only");
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php
$xml = simplexml_load_string("XXXXXXX^",$x,0x6000000000000001);
var_dump($xml);
?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d

Warning: simplexml_load_string(): Invalid options in %s on line %d
bool(false)
