--TEST--
bcdiv() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcdiv('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcdiv('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bcdiv(): Argument #1 ($num1) is not well-formed
ValueError: bcdiv(): Argument #2 ($num2) is not well-formed
