--TEST--
gmp_sqrtrem() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    $r = gmp_sqrtrem(-1);
    var_dump($r);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$r = gmp_sqrtrem("0");
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(2);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(10);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(7);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(3);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem(100000);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem("1000000");
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

$r = gmp_sqrtrem("1000001");
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

try {
    $n = gmp_init(-1);
    $r = gmp_sqrtrem($n);
    var_dump($r);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$n = gmp_init(1000001);
$r = gmp_sqrtrem($n);
var_dump(gmp_strval($r[0]));
var_dump(gmp_strval($r[1]));

try {
    var_dump(gmp_sqrtrem(array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp_sqrtrem(): Argument #1 ($num) must be greater than or equal to 0
string(1) "0"
string(1) "0"
string(1) "1"
string(1) "1"
string(1) "3"
string(1) "1"
string(1) "2"
string(1) "3"
string(1) "1"
string(1) "2"
string(3) "316"
string(3) "144"
string(4) "1000"
string(1) "0"
string(4) "1000"
string(1) "1"
gmp_sqrtrem(): Argument #1 ($num) must be greater than or equal to 0
string(4) "1000"
string(1) "1"
gmp_sqrtrem(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
