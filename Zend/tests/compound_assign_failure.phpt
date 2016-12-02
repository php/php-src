--TEST--
Behavior of failing compound assignment
--FILE--
<?php

try {
	$a = 1;
	$a %= 0;
} catch (Error $e) { var_dump($a); }

try {
	$a = 1;
	$a >>= -1;
} catch (Error $e) { var_dump($a); }

try {
	$a = 1;
	$a <<= -1;
} catch (Error $e) { var_dump($a); }
?>
--EXPECT--
int(1)
int(1)
int(1)
