--TEST--
ArrayObject when function ksort() is disabled
--INI--
disable_functions=ksort
--FILE--
<?php
$a = ['hello', 'world', 'SPL', 'is', 'fun', '!'];
$ao = new ArrayObject($a);
$ao->ksort();
var_dump($ao);

?>
--EXPECTF--
Fatal error: Uncaught Error: Invalid callback ksort, function "ksort" not found or invalid function name in %s:%d
Stack trace:
#0 %s(%d): ArrayObject->ksort()
#1 {main}
  thrown in %s on line %d
