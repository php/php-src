--TEST--
Typed local variables: compound assignment (+= .= etc.) enforces the declared type
--FILE--
<?php
// int += float in weak mode: deprecation + truncation to int (matches typed property)
int $x = 1;
$x += 0.5;
var_dump($x);

// .= keeps a string typed local a string
string $s = "a";
$s .= "b";
var_dump($s);

// float += int stays float
float $f = 1.0;
$f += 2;
var_dump($f);

// numeric-string RHS coerces to int
int $n = 10;
$n .= "5"; // becomes "105", coerced back to int
var_dump($n);

// bitwise / shift / modulo / power keep int
int $b = 12;
$b &= 6;
$b |= 1;
$b ^= 2;
$b <<= 1;
$b >>= 1;
$b %= 7;
$b **= 2;
var_dump($b);
?>
--EXPECTF--
Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)
string(2) "ab"
float(3)
int(105)
int(0)
