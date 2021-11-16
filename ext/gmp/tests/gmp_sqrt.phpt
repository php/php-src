--TEST--
gmp_sqrt() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    var_dump(gmp_strval(gmp_sqrt(-2)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_strval(gmp_sqrt("-2")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_strval(gmp_sqrt("0")));
var_dump(gmp_strval(gmp_sqrt("2")));
var_dump(gmp_strval(gmp_sqrt("144")));

$n = gmp_init(0);
var_dump(gmp_strval(gmp_sqrt($n)));
$n = gmp_init(-144);
try {
    var_dump(gmp_strval(gmp_sqrt($n)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
$n = gmp_init(777);
var_dump(gmp_strval(gmp_sqrt($n)));

try {
    var_dump(gmp_sqrt(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp_sqrt(): Argument #1 ($num) must be greater than or equal to 0
gmp_sqrt(): Argument #1 ($num) must be greater than or equal to 0
string(1) "0"
string(1) "1"
string(2) "12"
string(1) "0"
gmp_sqrt(): Argument #1 ($num) must be greater than or equal to 0
string(2) "27"
gmp_sqrt(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
