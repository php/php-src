--TEST--
Defining and using constants
--FILE--
<?php

define('foo', 	2);
define('1', 	2);
define(1, 		2);
define('',		1);
define('1foo',	3);

var_dump(constant('foo'));
var_dump(constant('1'));
var_dump(constant(1));
var_dump(constant(''));
var_dump(constant('1foo'));

?>
--EXPECTF--
Warning: Constant 1 already defined in %s on line %d
int(2)
int(2)
int(2)
int(1)
int(3)
