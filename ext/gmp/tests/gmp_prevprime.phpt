--TEST--
gmp_prevprime()
--EXTENSIONS--
gmp
--SKIPIF--
<?php
if (!function_exists('gmp_prevprime')) {
    die('skip gmp_prevprime() is not available');
}
?>
--FILE--
<?php

foreach ([-1, 0, 1, 2] as $value) {
    try {
        var_dump(gmp_prevprime($value));
    } catch (\ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

$definitelyPrime = null;
try {
    var_dump(gmp_prevprime(2, $definitelyPrime));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($definitelyPrime);

var_dump(gmp_strval(gmp_prevprime(3)));
var_dump(gmp_strval(gmp_prevprime(4)));
var_dump(gmp_strval(gmp_prevprime(10000)));

$definitelyPrime = null;
var_dump(gmp_strval(gmp_prevprime(3, $definitelyPrime)));
var_dump($definitelyPrime);

$probablePrime = gmp_nextprime(gmp_pow(10, 80));
$definitelyPrime = null;
$previousPrime = gmp_prevprime(gmp_add($probablePrime, 1), $definitelyPrime);
var_dump(gmp_cmp($previousPrime, $probablePrime) === 0);
var_dump(is_bool($definitelyPrime));
var_dump($definitelyPrime === (gmp_prob_prime($previousPrime) === 2));

?>
--EXPECT--
ValueError: gmp_prevprime(): Argument #1 ($num) must be greater than 2
ValueError: gmp_prevprime(): Argument #1 ($num) must be greater than 2
ValueError: gmp_prevprime(): Argument #1 ($num) must be greater than 2
ValueError: gmp_prevprime(): Argument #1 ($num) must be greater than 2
ValueError: gmp_prevprime(): Argument #1 ($num) must be greater than 2
NULL
string(1) "2"
string(1) "3"
string(4) "9973"
string(1) "2"
bool(true)
bool(true)
bool(true)
bool(true)
