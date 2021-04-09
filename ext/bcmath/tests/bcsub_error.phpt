--TEST--
bcsub() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcsub('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcsub('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcsub(): Argument #1 ($num1) is not well-formed
bcsub(): Argument #2 ($num2) is not well-formed
