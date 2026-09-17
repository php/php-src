--TEST--
bcmod() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcmod('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcmod('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcmod(): Argument #1 ($num1) is not well-formed
ValueError: bcmod(): Argument #2 ($num2) is not well-formed
