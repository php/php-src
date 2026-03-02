--TEST--
bcpowmod() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcpowmod('a', '1', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcpowmod('1', 'a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcpowmod('1', '1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcpowmod(): Argument #1 ($num) is not well-formed
bcpowmod(): Argument #2 ($exponent) is not well-formed
bcpowmod(): Argument #3 ($modulus) is not well-formed
