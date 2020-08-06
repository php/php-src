--TEST--
Bug #73532 (Null pointer dereference in mb_eregi)
--SKIPIF--
<?php
require 'skipif.inc';
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
try {
    var_dump(mb_eregi("a", "\xf5"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
mb_eregi(): Argument #2 ($string) must be a valid string in "UTF-8" encoding
