--TEST--
Error cases of compound shift assignment on strings
--FILE--
<?php

$n = "65";
$n <<= $n;
var_dump($n);

$n = "-1";
try {
    $n <<= $n;
    var_dump($n);
} catch (Exception $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$n = "65";
$n >>= $n;
var_dump($n);

$n = "-1";
try {
  $n >>= $n;
  var_dump($n);
} catch (Exception $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$n = "0";
try{
  $n %= $n;
  var_dump($n);
} catch (Exception $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

$n = "-1";
$n %= $n;
var_dump($n);
--EXPECTF--
int(0)

Exception: Bit shift by negative number
int(0)

Exception: Bit shift by negative number

Exception: Division by zero
int(0)
