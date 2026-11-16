--TEST--
bzip2.decompress stream filter doesn't reach hard limit
--EXTENSIONS--
bz2
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$file = __DIR__ . "/oom_bzip2_decompress_filter.bz2";
file_put_contents($file, bzcompress(str_repeat("\0", 12 * 1024 * 1024), 9));

ini_set("memory_limit", "4M");

$fp = fopen($file, "rb");
stream_filter_append($fp, "bzip2.decompress", STREAM_FILTER_READ);
var_dump(stream_get_contents($fp));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/oom_bzip2_decompress_filter.bz2");
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
