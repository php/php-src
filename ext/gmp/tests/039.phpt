--TEST--
gmp_init() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_init("98765678"));
var_dump(gmp_strval(gmp_init("98765678")));
var_dump(gmp_strval(gmp_init()));
var_dump(gmp_init());
var_dump(gmp_init(1,2,3,4));
var_dump(gmp_init(1,-1));
var_dump(gmp_init("",36));
var_dump(gmp_init("foo",3));
var_dump(gmp_strval(gmp_init("993247326237679187178",3)));

echo "Done\n";
?>
--EXPECTF--	
resource(%d) of type (GMP integer)
string(8) "98765678"

Warning: gmp_init() expects at least 1 parameter, 0 given in %s on line %d

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_init() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gmp_init() expects at most 2 parameters, 4 given in %s on line %d
NULL

Warning: gmp_init(): Bad base for conversion: -1 (should be between 2 and 36) in %s on line %d
bool(false)
bool(false)
bool(false)
string(1) "0"
Done
