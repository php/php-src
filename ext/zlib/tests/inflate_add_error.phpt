--TEST--
Test incremental inflate_add() error functionality
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

$badResource = fopen("php://memory", "r+");
try {
    var_dump(inflate_add($badResource, "test"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$resource = inflate_init(ZLIB_ENCODING_DEFLATE);
$badFlushType = 6789;
try {
    var_dump(inflate_add($resource, "test", $badFlushType));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
inflate_add(): Argument #1 ($context) must be of type InflateContext, resource given
inflate_add(): Argument #3 ($flush_mode) must be one of ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK, or ZLIB_FINISH
