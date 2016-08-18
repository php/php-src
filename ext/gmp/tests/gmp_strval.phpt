--TEST--
gmp_strval() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval());
var_dump(gmp_strval(""));
var_dump(gmp_strval("", ""));
var_dump(gmp_strval("", -1));
var_dump(gmp_strval(-1, ""));

$fp = fopen(__FILE__, "r");
var_dump(gmp_strval($fp));

$g = gmp_init("9765456");
var_dump(gmp_strval($g));
var_dump(gmp_strval($g, -1));
var_dump(gmp_strval($g, 100000));
var_dump(gmp_strval($g, 10));

$g = gmp_init("-3373333");
var_dump(gmp_strval($g));
var_dump(gmp_strval($g, -1));
var_dump(gmp_strval($g, 100000));
var_dump(gmp_strval($g, 10));

var_dump(gmp_strval(array(1,2), array(1,2)));
var_dump(gmp_strval(new stdclass, new stdclass));
var_dump(gmp_strval(array(1,2)));
var_dump(gmp_strval(new stdclass));

echo "Done\n";
?>
--EXPECTF--	
Warning: gmp_strval() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gmp_strval(): Unable to convert variable to GMP - string is not an integer in %s on line %d
bool(false)

Warning: gmp_strval() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: gmp_strval(): Bad base for conversion: -1 (should be between 2 and %d or -2 and -%d) in %s on line %d
bool(false)

Warning: gmp_strval() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
string(7) "9765456"

Warning: gmp_strval(): Bad base for conversion: -1 (should be between 2 and %d or -2 and -%d) in %s on line %d
bool(false)

Warning: gmp_strval(): Bad base for conversion: 100000 (should be between 2 and %d or -2 and -%d) in %s on line %d
bool(false)
string(7) "9765456"
string(8) "-3373333"

Warning: gmp_strval(): Bad base for conversion: -1 (should be between 2 and %d or -2 and -%d) in %s on line %d
bool(false)

Warning: gmp_strval(): Bad base for conversion: 100000 (should be between 2 and %d or -2 and -%d) in %s on line %d
bool(false)
string(8) "-3373333"

Warning: gmp_strval() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: gmp_strval() expects parameter 2 to be integer, object given in %s on line %d
NULL

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
