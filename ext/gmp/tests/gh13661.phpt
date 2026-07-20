--TEST--
GH-13661: Add gmp_prevprime()
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
        echo $e->getMessage() . \PHP_EOL;
    }
}

var_dump(gmp_strval(gmp_prevprime(3)));
var_dump(gmp_strval(gmp_prevprime(4)));
var_dump(gmp_strval(gmp_prevprime(10000)));

?>
--EXPECT--
gmp_prevprime(): Argument #1 ($num) must be greater than 2
gmp_prevprime(): Argument #1 ($num) must be greater than 2
gmp_prevprime(): Argument #1 ($num) must be greater than 2
gmp_prevprime(): Argument #1 ($num) must be greater than 2
string(1) "2"
string(1) "3"
string(4) "9973"
