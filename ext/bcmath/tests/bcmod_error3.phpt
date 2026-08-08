--TEST--
bcmod() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcmod('a', '1');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    bcmod('1', 'a');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: bcmod(): Argument #1 ($num1) is not well-formed
ValueError: bcmod(): Argument #2 ($num2) is not well-formed
