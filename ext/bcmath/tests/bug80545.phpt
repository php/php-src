--TEST--
Bug #80545 (bcadd('a', 'a') and bcadd('1', 'a') doesn't throw an exception)
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcadd('a', 'a');
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
