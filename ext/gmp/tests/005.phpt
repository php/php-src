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

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for gmp_strval() in %s on line %d
NULL
bool(false)
bool(false)
bool(false)

Warning: gmp_strval(): Bad base for conversion: 0 in %s on line %d
bool(false)

Warning: gmp_strval(): supplied resource is not a valid GMP integer resource in %s on line %d
bool(false)
string(7) "9765456"

Warning: gmp_strval(): Bad base for conversion: -1 in %s on line %d
bool(false)

Warning: gmp_strval(): Bad base for conversion: 100000 in %s on line %d
bool(false)
string(7) "9765456"
string(8) "-3373333"

Warning: gmp_strval(): Bad base for conversion: -1 in %s on line %d
bool(false)

Warning: gmp_strval(): Bad base for conversion: 100000 in %s on line %d
bool(false)
string(8) "-3373333"

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
