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
Warning: Wrong parameter count for gmp_mod() in %s on line %d
NULL

Warning: Wrong parameter count for gmp_mod() in %s on line %d
NULL
bool(false)
int(0)
resource(%d) of type (GMP integer)

Warning: gmp_mod(): Zero operand not allowed in %s on line %d
bool(false)

Warning: gmp_mod(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
resource(%d) of type (GMP integer)
Done
