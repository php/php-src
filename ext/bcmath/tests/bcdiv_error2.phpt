--TEST--
bcdiv() requires well-formed values
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php

try {
    bcdiv('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcdiv('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcdiv(): Argument #1 ($num1) is not well-formed
bcdiv(): Argument #2 ($num2) is not well-formed
