--TEST--
ArrayObject when function natcasesort() is disabled
--INI--
disable_functions=natcasesort
--FILE--
<?php
$a = ['hello', 'world', 'SPL', 'is', 'fun', '!'];
$ao = new ArrayObject($a);
$ao->natcasesort();
var_dump($ao);

?>
--EXPECTF--
Fatal error: Uncaught Error: Invalid callback natcasesort, function "natcasesort" not found or invalid function name in %s:%d
Stack trace:
#0 %s(%d): ArrayObject->natcasesort()
#1 {main}
  thrown in %s on line %d
