--TEST--
ArrayObject when function asort() is disabled
--INI--
disable_functions=asort
--FILE--
<?php
$a = ['hello', 'world', 'SPL', 'is', 'fun', '!'];
$ao = new ArrayObject($a);
$ao->asort();
var_dump($ao);

?>
--EXPECTF--
Fatal error: Uncaught Error: Invalid callback asort, function "asort" not found or invalid function name in %s:%d
Stack trace:
#0 %s(%d): ArrayObject->asort()
#1 {main}
  thrown in %s on line %d
