--TEST--
Test ob_start() with non existent callback method.
--FILE--
<?php
/* 
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/ 

Class C {
}

$c = new C;
var_dump(ob_start(array($c, 'f')));
echo "done"
?>
--EXPECTF--
Catchable fatal error: Object of class C could not be converted to string in %s on line 11