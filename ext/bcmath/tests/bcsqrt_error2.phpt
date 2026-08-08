--TEST--
bcsqrt() requires a well-formed value
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcsqrt('a');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: bcsqrt(): Argument #1 ($num) is not well-formed
