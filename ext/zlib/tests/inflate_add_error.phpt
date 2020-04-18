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
inflate_add(): supplied resource is not a valid zlib inflate resource
Flush mode must be ZLIB_NO_FLUSH, ZLIB_PARTIAL_FLUSH, ZLIB_SYNC_FLUSH, ZLIB_FULL_FLUSH, ZLIB_BLOCK or ZLIB_FINISH
