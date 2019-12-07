--TEST--
Bug #79149 (SEGV in mb_convert_encoding with non-string encodings)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--FILE--
<?php
try {
    var_dump(mb_convert_encoding("", "UTF-8", [0]));
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(mb_convert_encoding('foo', 'UTF-8', array(['bar'], ['baz'])));
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
Error: Illegal character encoding specified

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
Error: Illegal character encoding specified
