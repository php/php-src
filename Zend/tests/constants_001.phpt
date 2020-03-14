--TEST--
Defining and using constants
--FILE--
<?php

require __DIR__ . '/constants_helpers.inc';

tchelper_define('foo', 	2);
tchelper_define('1', 	2);
tchelper_define(1, 		2);
tchelper_define('',		1);
tchelper_define('1foo',	3);

var_dump(constant('foo'));
var_dump(constant('1'));
var_dump(constant(1));
var_dump(constant(''));
var_dump(constant('1foo'));

?>
--EXPECT--
>> define("foo", integer);
true

>> define("1", integer);
true

>> define(integer, integer);
ValueError :: Constant 1 already defined

>> define("", integer);
true

>> define("1foo", integer);
true

int(2)
int(2)
int(2)
int(1)
int(3)
