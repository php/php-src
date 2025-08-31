--TEST--
SPL: ArrayObject::offsetExists() should return true for element containing NULL
--FILE--
<?php
$ao = new ArrayObject(array('foo' => null));
var_dump($ao->offsetExists('foo'));

?>
--EXPECT--
bool(true)
