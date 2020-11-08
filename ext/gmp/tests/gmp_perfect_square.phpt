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

try {
    var_dump(gmp_perfect_square(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
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
gmp_perfect_square(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
