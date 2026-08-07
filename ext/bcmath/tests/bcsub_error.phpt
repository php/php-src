--TEST--
bcsub() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcsub('a', '1');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    bcsub('1', 'a');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: bcsub(): Argument #1 ($num1) is not well-formed
ValueError: bcsub(): Argument #2 ($num2) is not well-formed
