--TEST--
define() tests
--FILE--
<?php

var_dump(define());
var_dump(define("TRUE"));
var_dump(define("TRUE", 1));
var_dump(define("TRUE", 1, array(1)));

var_dump(define(array(1,2,3,4,5), 1));
var_dump(define(" ", 1));
var_dump(define("[[[", 2));
var_dump(define("test const", 3));
var_dump(define("test const", 3));
var_dump(define("test", array(1)));
var_dump(define("test1", new stdclass));

var_dump(constant(" "));
var_dump(constant("[[["));
var_dump(constant("test const"));

echo "Done\n";
?>
--EXPECTF--
Warning: define() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: define() expects at least 2 parameters, 1 given in %s on line %d
NULL
bool(true)

Warning: define() expects parameter 3 to be boolean, array given in %s on line %d
NULL

Warning: define() expects parameter 1 to be string, array given in %s on line %d
NULL
bool(true)
bool(true)
bool(true)

Notice: Constant test const already defined in %s on line %d
bool(false)
bool(true)

Warning: Constants may only evaluate to scalar values or arrays in %s on line %d
bool(false)
int(1)
int(2)
int(3)
Done
