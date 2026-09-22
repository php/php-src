--TEST--
GH-18304 (Changing the properties of a DateInterval through dynamic properties triggers a SegFault)
--CREDITS--
orose-assetgo
--FILE--
<?php
$ao = new ArrayObject(['abc' => 1]);
$ao->setFlags(ArrayObject::ARRAY_AS_PROPS);
$field = 'abc';
$ao->$field++;
var_dump($ao->$field);
?>
--EXPECT--
int(2)
