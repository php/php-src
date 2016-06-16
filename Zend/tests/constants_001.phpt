--TEST--
Defining and using constants
--FILE--
<?php

define('foo', 	2);
define('1', 	2);
try {
	define(1, 		2);
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
define('',		1);
define('1foo',	3);

var_dump(constant('foo'));
var_dump(constant('1'));
var_dump(constant(1));
var_dump(constant(''));
var_dump(constant('1foo'));

?>
--EXPECT--
Exception: Constant 1 already defined
int(2)
int(2)
int(2)
int(1)
int(3)
