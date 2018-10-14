--TEST--
Test incremental inflate_init() context reuse
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$resource = inflate_init(ZLIB_ENCODING_GZIP);

$uncompressed = implode(range("a","z"));
$compressed = gzencode($uncompressed);
$inflated = "";
for ($i=0;$i<strlen($compressed);$i++) {
    $inflated .= inflate_add($resource, $compressed[$i]);
}
$inflated .= inflate_add($resource, "", ZLIB_FINISH);
assert($inflated === $uncompressed);

// Now reuse the existing resource after finishing the previous operations ...
$inflated = "";
for ($i=0;$i<strlen($compressed);$i++) {
    $inflated .= inflate_add($resource, $compressed[$i]);
}
$inflated .= inflate_add($resource, "", ZLIB_FINISH);
assert($inflated === $uncompressed);
?>
===DONE===
--EXPECTF--
===DONE===
