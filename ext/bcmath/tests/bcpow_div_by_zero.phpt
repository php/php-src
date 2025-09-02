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
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

?>
--EXPECT--
Negative power of zero
Negative power of zero
Negative power of zero
Negative power of zero
Negative power of zero
Negative power of zero
