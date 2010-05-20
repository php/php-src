--TEST--
Parameter type hint - Testing with lambda function
--FILE--
<?php

$test = create_function('integer $i', 'return $i + 1;');
var_dump($test(1));
var_dump($test('foo'));

?>
--EXPECTF--
int(2)

Catchable fatal error: Argument 1 passed to __lambda_func() must be of the type integer, string given, called in %s on line %d and defined in %s(%d) : runtime-created function on line %d
--UEXPECTF--
int(2)

Catchable fatal error: Argument 1 passed to __lambda_func() must be of the type integer, Unicode string given, called in %s on line %d and defined in %s(%d) : runtime-created function on line %d
