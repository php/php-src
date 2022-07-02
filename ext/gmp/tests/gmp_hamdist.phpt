--TEST--
gmp_hamdist() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_hamdist(1231231, 0));
var_dump(gmp_hamdist(1231231, -1));
var_dump(gmp_hamdist(1231231, "8765434567897654333334567"));
var_dump(gmp_hamdist(-364264234, "8333765434567897654333334567"));

$n = gmp_init("8765434567");
$n1 = gmp_init("987654445678");
var_dump(gmp_hamdist($n, "8333765434567897654333334567"));
var_dump(gmp_hamdist($n, $n));
var_dump(gmp_hamdist($n, $n1));

try {
    var_dump(gmp_hamdist($n, array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_hamdist(array(), $n));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_hamdist(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
int(13)
int(-1)
int(36)
int(-1)
int(43)
int(0)
int(26)
gmp_hamdist(): Argument #2 ($num2) must be of type GMP|string|int, array given
gmp_hamdist(): Argument #1 ($num1) must be of type GMP|string|int, array given
gmp_hamdist(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
