--TEST--
Test incremental deflate_add() error functionality
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
deflate_add(): supplied resource is not a valid zlib deflate resource
Flush mode must be ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK or ZLIB_FINISH
