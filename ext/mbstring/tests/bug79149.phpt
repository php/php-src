--TEST--
Bug #79149 (SEGV in mb_convert_encoding with non-string encodings)
--EXTENSIONS--
mbstring
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
mb_convert_encoding(): Argument #3 ($from_encoding) contains invalid encoding "0"

Warning: Array to string conversion in %s on line %d
mb_convert_encoding(): Argument #3 ($from_encoding) contains invalid encoding "Array"
mb_convert_encoding(): Argument #3 ($from_encoding) contains invalid encoding "foo"
