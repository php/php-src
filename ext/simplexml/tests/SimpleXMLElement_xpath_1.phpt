--TEST--
Testing xpath() with invalid XML
--SKIPIF--
<?php
if (PHP_MAJOR_VERSION != 5) die("skip this test is for PHP 5 only");
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--FILE--
<?php
$xml = simplexml_load_string("XXXXXXX^",$x,0x6000000000000001);
var_dump($xml->xpath("BBBB"));
?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d

Warning: simplexml_load_string(): Entity: line 1: parser error : Start tag expected, '<' not found in %s on line %d

Warning: simplexml_load_string(): XXXXXXX^ in %s on line %d

Warning: simplexml_load_string(): ^ in %s on line %d

Fatal error: Call to a member function xpath() on boolean in %s on line %d
