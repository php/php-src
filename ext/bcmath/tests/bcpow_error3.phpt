--TEST--
bcpow() requires well-formed values
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php

try {
    bcpow('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcpow('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcpow(): Argument #1 ($num) is not well-formed
bcpow(): Argument #2 ($exponent) is not well-formed
