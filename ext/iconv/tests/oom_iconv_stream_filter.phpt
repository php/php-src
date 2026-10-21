--TEST--
convert.iconv stream filter doesn't reach hard limit
--EXTENSIONS--
iconv
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$file = __DIR__ . "/oom_iconv_stream_filter.txt";
file_put_contents($file, str_repeat("a", 4 * 1024 * 1024));

ini_set("memory_limit", "4M");

// UTF-8 to UTF-32 quadruples the input.
$fp = fopen($file, "rb");
stream_filter_append($fp, "convert.iconv.UTF-8/UTF-32", STREAM_FILTER_READ);
var_dump(stream_get_contents($fp));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/oom_iconv_stream_filter.txt");
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
