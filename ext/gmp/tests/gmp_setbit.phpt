--TEST--
gmp_setbit() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

$n = gmp_init(-1);
gmp_setbit($n, 10, true);
var_dump(gmp_strval($n));

$n = gmp_init(5);
try {
    gmp_setbit($n, -20, false);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_strval($n));

$n = gmp_init(5);
gmp_setbit($n, 2, false);
var_dump(gmp_strval($n));

$n = gmp_init(5);
gmp_setbit($n, 1, true);
var_dump(gmp_strval($n));

$n = gmp_init("100000000000");
gmp_setbit($n, 23, true);
var_dump(gmp_strval($n));

gmp_setbit($n, 23, false);
var_dump(gmp_strval($n));

gmp_setbit($n, 3);
var_dump(gmp_strval($n));

$b = "";
try {
    gmp_setbit($b, 23);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$a = array();
try {
    gmp_setbit($a, array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
string(2) "-1"
gmp_setbit(): Argument #2 ($index) must be between 0 and %d * %d
string(1) "5"
string(1) "1"
string(1) "7"
string(12) "100008388608"
string(12) "100000000000"
string(12) "100000000008"
gmp_setbit(): Argument #1 ($num) must be of type GMP, string given
gmp_setbit(): Argument #1 ($num) must be of type GMP, array given
Done
