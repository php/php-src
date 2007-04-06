--TEST--
Bug #40442 ()
--FILE--
<?php
$a = new ArrayObject();
$a->offsetSet('property', 0);
var_dump($a->offsetExists('property'));
?>
===DONE===
--EXPECT--
bool(true)
===DONE===
