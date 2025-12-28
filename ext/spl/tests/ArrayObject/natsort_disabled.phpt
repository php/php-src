--TEST--
ArrayObject when function natsort() is disabled
--INI--
disable_functions=natsort
--FILE--
<?php
$a = ['hello', 'world', 'SPL', 'is', 'fun', '!'];
$ao = new ArrayObject($a);
$ao->natsort();
var_dump($ao);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call method natsort when function natsort is disabled in %s:%d
Stack trace:
#0 %s(%d): ArrayObject->natsort()
#1 {main}
  thrown in %s on line %d
