--TEST--
gmp_popcount() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_popcount(-1));
var_dump(gmp_popcount(0));
var_dump(gmp_popcount(12123));
var_dump(gmp_popcount("52638927634234"));
var_dump(gmp_popcount("-23476123423433"));
$n = gmp_init("9876546789222");
var_dump(gmp_popcount($n));

try {
    var_dump(gmp_popcount(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
int(-1)
int(0)
int(10)
int(31)
int(-1)
int(20)
gmp_popcount(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
