--TEST--
bcadd() requires well-formed values
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
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
