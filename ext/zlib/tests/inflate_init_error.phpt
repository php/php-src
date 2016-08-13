--TEST--
Test inflate_init() error
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded"; 
}
?>
--FILE--
<?php
var_dump(inflate_init());
var_dump(inflate_init(42));
?>
--EXPECTF--

Warning: inflate_init() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: inflate_init(): encoding mode must be ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE in %s on line %d
bool(false)
