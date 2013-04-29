--TEST--
Bug #28822 (ArrayObject::offsetExists() works inverted)
--FILE--
<?php

$array = new ArrayObject();
$array->offsetSet('key', 'value');
var_dump($array->offsetExists('key'));
var_dump($array->offsetExists('nokey'));

?>
===DONE===
--EXPECT--
bool(true)
bool(false)
===DONE===
