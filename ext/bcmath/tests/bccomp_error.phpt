--TEST--
bccomp() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bccomp('a', '1');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    bccomp('1', 'a');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: bccomp(): Argument #1 ($num1) is not well-formed
ValueError: bccomp(): Argument #2 ($num2) is not well-formed
