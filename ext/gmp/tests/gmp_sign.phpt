--TEST--
gmp_sign() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_sign(-1));
var_dump(gmp_sign(1));
var_dump(gmp_sign(0));
var_dump(gmp_sign("123718235123123"));
var_dump(gmp_sign("-34535345345"));

try {
    var_dump(gmp_sign("+34534573457345"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $n = gmp_init("098909878976786545");
    var_dump(gmp_sign($n));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_sign(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
int(-1)
int(1)
int(0)
int(1)
int(-1)
gmp_sign(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
gmp_sign(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
