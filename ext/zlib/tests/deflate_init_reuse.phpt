--TEST--
Test incremental deflate_init() context reuse
--EXTENSIONS--
zlib
--FILE--
<?php
$resource = deflate_init(ZLIB_ENCODING_DEFLATE);
foreach (range("a", "z") as $char) {
    deflate_add($resource, $char);
}
deflate_add($resource, "", ZLIB_FINISH);

// Now reuse the existing resource after finishing the previous operations ...
$uncompressed = $compressed = "";
foreach (range("a", "z") as $char) {
    $uncompressed .= $char;
    $compressed .= deflate_add($resource, $char, ZLIB_NO_FLUSH);
}
$compressed .= deflate_add($resource, "", ZLIB_FINISH);
assert($uncompressed === zlib_decode($compressed));
?>
===DONE===
--EXPECT--
===DONE===
