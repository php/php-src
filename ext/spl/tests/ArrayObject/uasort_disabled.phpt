--TEST--
ArrayObject when function uasort() is disabled
--INI--
disable_functions=uasort
--FILE--
<?php
$a = ['hello', 'world', 'SPL', 'is', 'fun', '!'];
$ao = new ArrayObject($a);
$ao->uasort(fn ($l, $r) => $l <=> $r);
var_dump($ao);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call method uasort when function uasort is disabled in %s:%d
Stack trace:
#0 %s(%d): ArrayObject->uasort(Object(Closure))
#1 {main}
  thrown in %s on line %d
