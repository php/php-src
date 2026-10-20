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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$n = "65";
$n >>= $n;
var_dump($n);

$n = "-1";
try {
  $n >>= $n;
  var_dump($n);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$n = "0";
try{
  $n %= $n;
  var_dump($n);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$n = "-1";
$n %= $n;
var_dump($n);
?>
--EXPECT--
int(0)
ArithmeticError: Bit shift by negative number
int(0)
ArithmeticError: Bit shift by negative number
DivisionByZeroError: Modulo by zero
int(0)
