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
Warning: Wrong parameter count for define() in %s on line %d
NULL

Warning: Wrong parameter count for define() in %s on line %d
NULL
bool(true)

Notice: Constant true already defined in %s on line %d
bool(false)

Notice: Array to string conversion in %s on line %d
bool(true)
bool(true)
bool(true)
bool(true)

Notice: Constant test const already defined in %s on line %d
bool(false)

Warning: Constants may only evaluate to scalar values in %s on line %d
bool(false)

Warning: Constants may only evaluate to scalar values in %s on line %d
bool(false)
int(1)
int(2)
int(3)
Done
