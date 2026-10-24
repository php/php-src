--TEST--
bcdivmod() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcdivmod('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcdivmod('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcdivmod(): Argument #1 ($num1) is not well-formed
bcdivmod(): Argument #2 ($num2) is not well-formed
