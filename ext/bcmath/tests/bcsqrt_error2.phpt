--TEST--
bcsqrt() requires a well-formed value
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
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
