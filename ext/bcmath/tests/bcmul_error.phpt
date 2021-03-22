--TEST--
bcmul() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcmul('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcmul('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcmul(): Argument #1 ($num1) is not well-formed
bcmul(): Argument #2 ($num2) is not well-formed
