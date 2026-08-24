--TEST--
bcadd() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcadd('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcadd('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcadd(): Argument #1 ($num1) is not well-formed
ValueError: bcadd(): Argument #2 ($num2) is not well-formed
