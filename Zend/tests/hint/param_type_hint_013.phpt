--TEST--
Parameter type hint - Testing with lambda function using 2 parameters
--FILE--
<?php

$test = create_function('integer $i, string $j', 'return $i + $j;');
var_dump($test(1, 'foo'));
var_dump($test(1, '1'));
var_dump($test(1, NULL));

?>
--EXPECTF--
int(1)
int(2)

Catchable fatal error: Argument 2 passed to __lambda_func() must be of the type string, null given, called in %s on line %d and defined in %s(%d) : runtime-created function on line %d
