--TEST--
Bug #48693 (Double declaration of __lambda_func when lambda wrongly formatted)
--FILE--
<?php

$x = create_function('', 'return 1; }');
$y = create_function('', 'function a() { }; return 2;');
$z = create_function('', '{');
$w = create_function('', 'return 3;');

var_dump(
	$x,
	$y(),
	$z,
	$w(),
	$y != $z
);

?>
--EXPECTF--
Parse error: %s in %s(%d) : runtime-created function on line 1

Parse error: %s %s(%d) : runtime-created function on line 1
bool(false)
int(2)
bool(false)
int(3)
bool(true)
