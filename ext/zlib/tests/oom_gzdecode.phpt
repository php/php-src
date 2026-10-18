--TEST--
gzdecode() doesn't reach hard limit
--EXTENSIONS--
zlib
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// Expands to 32 MB, well past the 16 MB hard limit.
$ctx = deflate_init(ZLIB_ENCODING_GZIP, ["level" => 9]);
$chunk = str_repeat("\0", 65536);
$gz = "";
for ($i = 0; $i < 512; $i++) {
    $gz .= deflate_add($ctx, $chunk, ZLIB_NO_FLUSH);
}
$gz .= deflate_add($ctx, "", ZLIB_FINISH);
unset($chunk);

var_dump(gzdecode($gz));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
