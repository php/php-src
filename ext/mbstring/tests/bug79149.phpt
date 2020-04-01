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
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_convert_encoding('foo', 'UTF-8', array(['bar'], ['baz'])));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(mb_convert_encoding('foo', 'UTF-8', array("foo\0bar")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
mb_convert_encoding(): Argument #3 ($from) must be an array of valid encoding, "0" given

Warning: Array to string conversion in %s on line %d
mb_convert_encoding(): Argument #3 ($from) must be an array of valid encoding, "Array" given
mb_convert_encoding(): Argument #3 ($from) must be an array of valid encoding, "foo" given
