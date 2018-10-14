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
var_dump(deflate_add($badResource, "test"));

$resource = deflate_init(ZLIB_ENCODING_DEFLATE);
$badFlushType = 6789;
var_dump(deflate_add($resource, "test", $badFlushType));
?>
--EXPECTF--
Warning: deflate_add(): Invalid deflate resource in %s on line %d
bool(false)

Warning: deflate_add(): flush mode must be ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK or ZLIB_FINISH in %s on line %d
bool(false)
