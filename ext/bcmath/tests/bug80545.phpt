--TEST--
Bug #80545 (bcadd('a', 'a') and bcadd('1', 'a') doesn't throw an exception)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php

try {
    bcadd('a', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    bcadd('1', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
bcadd(): Argument #1 ($num1) bcmath function argument is not well-formed
bcadd(): Argument #2 ($num2) bcmath function argument is not well-formed