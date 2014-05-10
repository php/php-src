--TEST--
Testing xpath() with invalid XML
--FILE--
<?php
$xml = @simplexml_load_string("XXXXXXX^",$x,0x6000000000000001);
var_dump($xml->xpath("BBBB"));
--EXPECT--
bool(false)
