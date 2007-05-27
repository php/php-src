--TEST--
gmp_sqrt() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_sqrt(-2)));
var_dump(gmp_strval(gmp_sqrt("-2")));
var_dump(gmp_strval(gmp_sqrt("0")));
var_dump(gmp_strval(gmp_sqrt("2")));
var_dump(gmp_strval(gmp_sqrt("144")));

$n = gmp_init(0);
var_dump(gmp_strval(gmp_sqrt($n)));
$n = gmp_init(-144);
var_dump(gmp_strval(gmp_sqrt($n)));
$n = gmp_init(777);
var_dump(gmp_strval(gmp_sqrt($n)));

var_dump(gmp_sqrt($n, 1));
var_dump(gmp_sqrt());
var_dump(gmp_sqrt(array()));

echo "Done\n";
?>
--EXPECTF--	
Warning: gmp_sqrt(): Number has to be greater than or equal to 0 in %s on line %d
string(1) "0"

Warning: gmp_sqrt(): Number has to be greater than or equal to 0 in %s on line %d
string(1) "0"
string(1) "0"
string(1) "1"
string(2) "12"
string(1) "0"

Warning: gmp_sqrt(): Number has to be greater than or equal to 0 in %s on line %d
string(1) "0"
string(2) "27"

Warning: Wrong parameter count for gmp_sqrt() in %s on line %d
NULL

Warning: Wrong parameter count for gmp_sqrt() in %s on line %d
NULL

Warning: gmp_sqrt(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
