--TEST--
bcsub() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcsub('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcsub('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcsub(): Argument #1 ($num1) is not well-formed
ValueError: bcsub(): Argument #2 ($num2) is not well-formed
