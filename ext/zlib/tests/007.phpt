--TEST--
gzencode() and invalid params
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    var_dump(gzencode("", -10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gzencode("", 100));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gzencode("", 1, 100));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gzencode("", -1, ZLIB_ENCODING_GZIP));
var_dump(gzencode("", 9, ZLIB_ENCODING_DEFLATE));

$string = "Light of my sun
Light in this temple
Light in my truth
Lies in the darkness";

try {
    var_dump(gzencode($string, 9, 3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gzencode($string, -1, ZLIB_ENCODING_GZIP));
var_dump(gzencode($string, 9, ZLIB_ENCODING_DEFLATE));

?>
--EXPECTF--
gzencode(): Argument #2 ($level) must be between -1 and 9
gzencode(): Argument #2 ($level) must be between -1 and 9
gzencode(): Argument #3 ($encoding) must be one of ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP, or ZLIB_ENCODING_DEFLATE
string(%d) "%a"
string(%d) "%a"
gzencode(): Argument #3 ($encoding) must be one of ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP, or ZLIB_ENCODING_DEFLATE
string(%d) "%a"
string(%d) "%a"
