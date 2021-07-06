--TEST--
Test inflate_init() error
--EXTENSIONS--
zlib
--FILE--
<?php

try {
    var_dump(inflate_init(42));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Encoding mode must be ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE
