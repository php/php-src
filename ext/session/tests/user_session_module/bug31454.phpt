--TEST--
Bug #31454 (session_set_save_handler crashes PHP when supplied non-existent object ref)
--EXTENSIONS--
session
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
--EXPECTF--
Deprecated: Calling session_set_save_handler() with more than 2 arguments is deprecated in %s on line %d
session_set_save_handler(): Argument #1 ($open) must be a valid callback, first array member is not a valid class name or object
Done
