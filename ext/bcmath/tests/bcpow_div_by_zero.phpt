--TEST--
bcpow() negative power of zero
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
$exponents = ["-15", "-1", "-9"];
$baseNumbers = ['0', '-0'];

foreach ($baseNumbers as $baseNumber) {
    foreach ($exponents as $exponent) {
        try {
            echo bcpow($baseNumber, $exponent), "\n";
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}

?>
--EXPECT--
DivisionByZeroError: Negative power of zero
DivisionByZeroError: Negative power of zero
DivisionByZeroError: Negative power of zero
DivisionByZeroError: Negative power of zero
DivisionByZeroError: Negative power of zero
DivisionByZeroError: Negative power of zero
