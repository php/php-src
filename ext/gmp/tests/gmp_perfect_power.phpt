--TEST--
gmp_perfect_power(): Check if number is a perfect power
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_perfect_power(0));
var_dump(gmp_perfect_power(1));
var_dump(gmp_perfect_power(2));
var_dump(gmp_perfect_power(4));
var_dump(gmp_perfect_power(6));
var_dump(gmp_perfect_power(8));
echo "\n";

var_dump(gmp_perfect_power(-1));
var_dump(gmp_perfect_power(-2));
var_dump(gmp_perfect_power(-4));
var_dump(gmp_perfect_power(-6));
var_dump(gmp_perfect_power(-8));
echo "\n";

$n = gmp_init("7442665456261594668083173595997");
var_dump(gmp_perfect_power($n));
var_dump(gmp_perfect_power($n+1));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)

bool(true)
bool(false)
bool(false)
bool(false)
bool(true)

bool(true)
bool(false)
