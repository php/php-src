--TEST--
bccomp() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bccomp('a', '1');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bccomp('1', 'a');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: bccomp(): Argument #1 ($num1) is not well-formed
ValueError: bccomp(): Argument #2 ($num2) is not well-formed
