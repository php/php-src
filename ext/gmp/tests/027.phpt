--TEST--
gmp_sign() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_sign(-1));
var_dump(gmp_sign(1));
var_dump(gmp_sign(0));
var_dump(gmp_sign("123718235123123"));
var_dump(gmp_sign("-34535345345"));
var_dump(gmp_sign("+34534573457345"));
$n = gmp_init("098909878976786545");
var_dump(gmp_sign($n));
var_dump(gmp_sign($n, $n));
var_dump(gmp_sign(array()));
var_dump(gmp_sign());

echo "Done\n";
?>
--EXPECTF--	
int(-1)
int(1)
int(0)
int(1)
int(-1)
bool(false)
int(0)

Warning: Wrong parameter count for gmp_sign() in %s on line %d
NULL

Warning: gmp_sign(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: Wrong parameter count for gmp_sign() in %s on line %d
NULL
Done
