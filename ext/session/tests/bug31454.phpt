--TEST--
Bug #31454 (session_set_save_handler crashes PHP when supplied non-existent object ref)
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

try {
    session_set_save_handler(
        array(&$arf, 'open'),
        array(&$arf, 'close'),
        array(&$arf, 'read'),
        array(&$arf, 'write'),
        array(&$arf, 'destroy'),
        array(&$arf, 'gc')
    );
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
session_set_save_handler(): Argument #1 ($open) must be a valid callback, function "Array" not found or invalid function name
Done
