--TEST--
GH-22993: DateInterval unserialize() error message with embedded NUL byte
--FILE--
<?php

try {
    unserialize('O:12:"DateInterval":2:{s:11:"from_string";b:1;s:11:"date_string";s:7:"foo' . "\0" . 'bar";}');
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Unknown or bad format (foo%0bar) at position 0 (f) while unserializing: The timezone could not be found in the database
