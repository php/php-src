--TEST--
gmp_cmp() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_cmp(123123,-123123));
var_dump(gmp_cmp("12345678900987654321","12345678900987654321"));
var_dump(gmp_cmp("12345678900987654321","123456789009876543211"));
var_dump(gmp_cmp(0,0));
var_dump(gmp_cmp(1231222,0));
var_dump(gmp_cmp(0,345355));

$n = gmp_init("827278512385463739");
var_dump(gmp_cmp(0,$n) < 0);
$n1 = gmp_init("827278512385463739");
var_dump(gmp_cmp($n1,$n));

var_dump(gmp_cmp($n1,$n,1));
var_dump(gmp_cmp(array(),array()));
var_dump(gmp_cmp(array()));
var_dump(gmp_cmp());

echo "Done\n";
?>
--EXPECTF--
int(2)
int(0)
int(-1)
int(0)
int(1)
int(-1)
bool(true)
int(0)

Warning: gmp_cmp() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: gmp_cmp(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_cmp() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: gmp_cmp() expects exactly 2 parameters, 0 given in %s on line %d
NULL
Done
