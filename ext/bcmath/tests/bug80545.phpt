--TEST--
Bug #80545 (bcadd('a', 'a') and bcadd('1', 'a') doesn't throw an exception)
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcadd('a', 'a');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    bcadd('1', 'a');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: bcadd(): Argument #1 ($num1) is not well-formed
ValueError: bcadd(): Argument #2 ($num2) is not well-formed
