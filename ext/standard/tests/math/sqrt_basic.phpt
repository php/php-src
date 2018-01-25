--TEST--
Test return type and value for expected input sqrt()
--INI--
precision = 14
--FILE--
<?php
/*
 * proto float sqrt(float number)
 * Function is implemented in ext/standard/math.c
*/

$arg_0 = 9.0;

var_dump(sqrt($arg_0));

?>
--EXPECT--
float(3)
