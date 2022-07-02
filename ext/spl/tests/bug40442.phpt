--TEST--
Bug #40442 (ArrayObject::offsetExists broke in 5.2.1, works in 5.2.0)
--FILE--
<?php
$a = new ArrayObject();
$a->offsetSet('property', 0);
var_dump($a->offsetExists('property'));
?>
--EXPECT--
bool(true)
