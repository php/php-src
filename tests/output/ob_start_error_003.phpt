--TEST--
Test ob_start() with object supplied but no method.
--FILE--
<?php
/* 
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/ 

Class C {
}

$c = new C;
var_dump(ob_start(array($c)));
echo "done"
?>
--EXPECTF--
Fatal error: ob_start(): No method name given: use ob_start(array($object,'method')) to specify instance $object and the name of a method of class C to use as output handler in %s on line 11