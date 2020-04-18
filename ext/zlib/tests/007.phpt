--TEST--
gzencode() and invalid params
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
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
Compression level (-10) must be within -1..9
Compression level (100) must be within -1..9
Encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE
string(%d) "%a"
string(%d) "%a"
Encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE
string(%d) "%a"
string(%d) "%a"
