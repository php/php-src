--TEST--
Testing xpath() with invalid XML
--FILE--
<?php
$xml = simplexml_load_string("XXXXXXX^",$x,0x6000000000000001);
var_dump($xml->xpath("BBBB"));
--EXPECTF--
Notice: Undefined variable: x in %s on line %d

Warning: simplexml_load_string() expects parameter 3 to be long, double given in %s on line %d

Catchable fatal error: Call to a member function xpath() on null in %s on line %d