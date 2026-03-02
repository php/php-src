--TEST--
ArrayObject when function uksort() is disabled
--INI--
disable_functions=uksort
--FILE--
<?php
$a = ['hello', 'world', 'SPL', 'is', 'fun', '!'];
$ao = new ArrayObject($a);
$ao->uksort(fn ($l, $r) => $l <=> $r);
var_dump($ao);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call method uksort when function uksort is disabled in %s:%d
Stack trace:
#0 %s(%d): ArrayObject->uksort(Object(Closure))
#1 {main}
  thrown in %s on line %d
