--TEST--
Behavior of failing compound assignment
--INI--
opcache.optimization_level=0
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

set_error_handler(function() { throw new Exception; });

try {
	$a = [];
	$a .= "foo";
} catch (Throwable $e) { var_dump($a); }

try {
	$a = "foo";
	$a .= [];
} catch (Throwable $e) { var_dump($a); }
?>
--EXPECT--
int(1)
int(1)
int(1)
array(0) {
}
string(3) "foo"
