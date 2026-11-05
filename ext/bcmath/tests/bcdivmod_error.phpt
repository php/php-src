--TEST--
bcdivmod() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcdivmod('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcdivmod('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcdivmod(): Argument #1 ($num1) is not well-formed
ValueError: bcdivmod(): Argument #2 ($num2) is not well-formed
