--TEST--
bccomp() requires well-formed values
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php

try {
    bccomp('a', '1');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bccomp('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bccomp(): Argument #1 ($num1) is not well-formed
bccomp(): Argument #2 ($num2) is not well-formed
