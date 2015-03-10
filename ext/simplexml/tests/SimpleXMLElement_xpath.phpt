--TEST--
Testing xpath() with invalid XML
--SKIPIF--
<?php PHP_INT_SIZE == 4 or die("skip - 32-bit only");
--FILE--
<?php
$xml = simplexml_load_string("XXXXXXX^",$x,0x6000000000000001);
var_dump($xml->xpath("BBBB"));
--EXPECTF--
Notice: Undefined variable: x in %s on line %d

Warning: simplexml_load_string() expects parameter 3 to be integer, float given in %s on line %d

Fatal error: Call to a member function xpath() on null in %s on line %d
