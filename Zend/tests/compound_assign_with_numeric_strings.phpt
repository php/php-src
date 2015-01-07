--TEST--
Error cases of compound shift assignment on strings
--FILE--
<?php

$n = "65";
$n <<= $n;
var_dump($n);

$n = "-1";
$n <<= $n;
var_dump($n);

$n = "65";
$n >>= $n;
var_dump($n);

$n = "-1";
$n >>= $n;
var_dump($n);

$n = "0";
$n %= $n;
var_dump($n);

$n = "-1";
$n %= $n;
var_dump($n);
--EXPECTF--
int(0)

Warning: Bit shift by negative number in %s on line %d
bool(false)
int(0)

Warning: Bit shift by negative number in %s on line %d
bool(false)

Warning: Division by zero in %s on line %d
bool(false)
int(0)
