--TEST--
bcpow() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcpow('a', '1');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    bcpow('1', 'a');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: bcpow(): Argument #1 ($num) is not well-formed
ValueError: bcpow(): Argument #2 ($exponent) is not well-formed
