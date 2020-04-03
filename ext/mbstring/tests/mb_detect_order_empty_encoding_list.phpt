--TEST--
Test mb_detect_order() function : empty encoding list
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
?>
--FILE--
<?php

try {
    var_dump( mb_detect_order(''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( mb_detect_order([]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_detect_order(): Argument #1 ($encoding) must specify at least one encoding
mb_detect_order(): Argument #1 ($encoding) must specify at least one encoding
