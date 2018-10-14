--TEST--
gmp_mod tests()
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_mod());
var_dump(gmp_mod(""));
var_dump(gmp_mod("",""));
var_dump(gmp_mod(0,1));
var_dump(gmp_mod(0,-1));
var_dump(gmp_mod(-1,0));

var_dump(gmp_mod(array(), array()));

$a = gmp_init("-100000000");
$b = gmp_init("353467");

var_dump(gmp_mod($a, $b));

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_mod() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: gmp_mod() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: gmp_mod(): Unable to convert variable to GMP - string is not an integer in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}

Warning: gmp_mod(): Zero operand not allowed in %s on line %d
bool(false)

Warning: gmp_mod(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(5) "31161"
}
Done
