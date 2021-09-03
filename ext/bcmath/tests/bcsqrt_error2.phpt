--TEST--
bcsqrt() requires a well-formed value
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcsqrt('a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcsqrt(): Argument #1 ($num) is not well-formed
