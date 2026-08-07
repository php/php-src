--TEST--
Test incremental deflate_add() error functionality
--EXTENSIONS--
zlib
--FILE--
<?php

$resource = deflate_init(ZLIB_ENCODING_DEFLATE);
$badFlushType = 6789;

try {
    var_dump(deflate_add($resource, "test", $badFlushType));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: deflate_add(): Argument #3 ($flush_mode) must be one of ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK, or ZLIB_FINISH
