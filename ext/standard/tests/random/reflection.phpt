--TEST--
Bug #74708 Wrong reflection on random_bytes and random_int
--FILE--
<?php
$rf = new ReflectionFunction('random_bytes');
var_dump($rf->getNumberOfParameters());
var_dump($rf->getNumberOfRequiredParameters());

$rf = new ReflectionFunction('random_int');
var_dump($rf->getNumberOfParameters());
var_dump($rf->getNumberOfRequiredParameters());
?>
===DONE===
--EXPECT--
int(1)
int(1)
int(2)
int(2)
===DONE===
