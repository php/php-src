--TEST--
gmp_neg() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_intval(gmp_neg(0)));
var_dump(gmp_intval(gmp_neg(1)));
var_dump(gmp_intval(gmp_neg(-1)));
var_dump(gmp_intval(gmp_neg("-1")));

try {
    var_dump(gmp_intval(gmp_neg("")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_intval(gmp_neg(0)));

$n = gmp_init("0");
var_dump(gmp_intval(gmp_neg($n)));
$n = gmp_init("12345678901234567890");
var_dump(gmp_strval(gmp_neg($n)));

try {
    var_dump(gmp_neg(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
int(0)
int(-1)
int(1)
int(1)
gmp_neg(): Argument #1 ($num) is not an integer string
int(0)
int(0)
string(21) "-12345678901234567890"
gmp_neg(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
