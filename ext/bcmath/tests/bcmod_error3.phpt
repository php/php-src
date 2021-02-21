--TEST--
bcmod() requires well-formed values
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php

try {
    bcmod('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcmod('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bcmod(): Argument #1 ($num1) is not well-formed
bcmod(): Argument #2 ($num2) is not well-formed
