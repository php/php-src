--TEST--
gmp_init() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_init("98765678"));
var_dump(gmp_strval(gmp_init("98765678")));
var_dump(gmp_init(1,-1));
var_dump(gmp_init("",36));
var_dump(gmp_init("foo",3));
var_dump(gmp_strval(gmp_init("993247326237679187178",3)));

echo "Done\n";
?>
--EXPECTF--
object(GMP)#%d (1) {
  ["num"]=>
  string(8) "98765678"
}
string(8) "98765678"

Warning: gmp_init(): Bad base for conversion: -1 (should be between 2 and %d) in %s on line %d
bool(false)

Warning: gmp_init(): Unable to convert variable to GMP - string is not an integer in %s on line %d
bool(false)

Warning: gmp_init(): Unable to convert variable to GMP - string is not an integer in %s on line %d
bool(false)

Warning: gmp_init(): Unable to convert variable to GMP - string is not an integer in %s on line %d
string(1) "0"
Done
