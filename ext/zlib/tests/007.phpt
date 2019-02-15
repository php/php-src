--TEST--
gzencode() and invalid params
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

var_dump(gzencode());
var_dump(gzencode(1,1,1,1));
var_dump(gzencode("", -10));
var_dump(gzencode("", 100));
var_dump(gzencode("", 1, 100));

var_dump(gzencode("", -1, ZLIB_ENCODING_GZIP));
var_dump(gzencode("", 9, ZLIB_ENCODING_DEFLATE));

$string = "Light of my sun
Light in this temple
Light in my truth
Lies in the darkness";

var_dump(gzencode($string, 9, 3));

var_dump(gzencode($string, -1, ZLIB_ENCODING_GZIP));
var_dump(gzencode($string, 9, ZLIB_ENCODING_DEFLATE));

echo "Done\n";
?>
--EXPECTF--
Warning: gzencode() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gzencode() expects at most 3 parameters, 4 given in %s on line %d
NULL

Warning: gzencode(): compression level (-10) must be within -1..9 in %s on line %d
bool(false)

Warning: gzencode(): compression level (100) must be within -1..9 in %s on line %d
bool(false)

Warning: gzencode(): encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE in %s on line %d
bool(false)
string(%d) "%a"
string(%d) "%a"

Warning: gzencode(): encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE in %s on line %d
bool(false)
string(%d) "%a"
string(%d) "%a"
Done
