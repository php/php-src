--TEST--
bcsqrt() requires a well-formed value
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcsqrt('a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcsqrt(): Argument #1 ($num) is not well-formed
