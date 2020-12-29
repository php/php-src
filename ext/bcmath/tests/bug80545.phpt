--TEST--
Bug #80545 (bcadd('a', 'a') doesn't throw an exception)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php
try {
    bcadd('a', 'a');
} catch (\ValueError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
bcadd(): Argument #1 ($num1) bcmath function argument is not well-formed