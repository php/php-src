--TEST--
Test incremental deflate_add() error functionality
--EXTENSIONS--
zlib
--FILE--
<?php
$badResource = fopen("php://memory", "r+");

try {
    var_dump(deflate_add($badResource, "test"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$resource = deflate_init(ZLIB_ENCODING_DEFLATE);
$badFlushType = 6789;

try {
    var_dump(deflate_add($resource, "test", $badFlushType));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
deflate_add(): Argument #1 ($context) must be of type DeflateContext, resource given
deflate_add(): Argument #3 ($flush_mode) must be one of ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK, or ZLIB_FINISH
