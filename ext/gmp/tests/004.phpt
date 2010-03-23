--TEST--
gmp_intval() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_intval(1,1));
var_dump(gmp_intval(""));
var_dump(gmp_intval(1.0001));
var_dump(gmp_intval("1.0001"));
var_dump(gmp_intval("-1"));
var_dump(gmp_intval(-1));
var_dump(gmp_intval(-2349828));
var_dump(gmp_intval(2342344));
var_dump(gmp_intval(new stdclass));
var_dump(gmp_intval(array()));

$fp = fopen(__FILE__, 'r');
var_dump(gmp_intval($fp));

$g = gmp_init("12345678");
var_dump(gmp_intval($g));

echo "Done\n";
?>
--EXPECTF--	
Warning: gmp_intval() expects exactly 1 parameter, 2 given in %s on line %d
NULL
int(0)
int(1)
int(1)
int(-1)
int(-1)
int(-2349828)
int(2342344)

Notice: Object of class stdClass could not be converted to int in %s on line %d
int(1)
int(0)

Warning: gmp_intval(): supplied resource is not a valid GMP integer resource in %s on line %d
bool(false)
int(12345678)
Done
