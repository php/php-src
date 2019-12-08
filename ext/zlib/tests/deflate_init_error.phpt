--TEST--
Test deflate_init() error
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

try {
    var_dump(deflate_init(42));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => 42]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => -2]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => 0]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => 10]));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
encoding mode must be ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE
compression level (42) must be within -1..9
compression level (-2) must be within -1..9
compression memory level (0) must be within 1..9
compression memory level (10) must be within 1..9
