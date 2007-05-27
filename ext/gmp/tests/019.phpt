--TEST--
gmp_perfect_square() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_perfect_square(0));
var_dump(gmp_perfect_square("0"));
var_dump(gmp_perfect_square(-1));
var_dump(gmp_perfect_square(1));
var_dump(gmp_perfect_square(16));
var_dump(gmp_perfect_square(17));
var_dump(gmp_perfect_square("1000000"));
var_dump(gmp_perfect_square("1000001"));

$n = gmp_init(100101);
var_dump(gmp_perfect_square($n));
$n = gmp_init(64);
var_dump(gmp_perfect_square($n));
$n = gmp_init(-5);
var_dump(gmp_perfect_square($n));

var_dump(gmp_perfect_square());
var_dump(gmp_perfect_square(array()));

echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)

Warning: Wrong parameter count for gmp_perfect_square() in %s on line %d
NULL

Warning: gmp_perfect_square(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
