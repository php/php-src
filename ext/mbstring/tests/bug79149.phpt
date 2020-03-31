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
Warning: mb_convert_encoding(): Unknown encoding "0" in %s on line %d
bool(false)

Warning: Array to string conversion in %s on line %d

Warning: mb_convert_encoding(): Unknown encoding "Array" in %s on line %d
bool(false)

Warning: mb_convert_encoding(): Unknown encoding "foo" in %s on line %d
bool(false)
