--TEST--
bcadd() requires well-formed values
--EXTENSIONS--
bcmath
--FILE--
<?php

try {
    bcadd('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcadd('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcadd(): Argument #1 ($num1) is not well-formed
bcadd(): Argument #2 ($num2) is not well-formed
