--TEST--
gzfile() doesn't reach hard limit
--EXTENSIONS--
zlib
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$file = __DIR__ . "/oom_gzfile.gz";

// Build 12 MB of one-byte lines as a ~12 KB gzip file without holding it in memory.
$ctx = deflate_init(ZLIB_ENCODING_GZIP, ["level" => 9]);
$chunk = str_repeat("a\n", 32768);
$gz = "";
for ($i = 0; $i < 192; $i++) {
    $gz .= deflate_add($ctx, $chunk, ZLIB_NO_FLUSH);
}
$gz .= deflate_add($ctx, "", ZLIB_FINISH);
file_put_contents($file, $gz);
unset($gz, $chunk, $ctx);

ini_set("memory_limit", "8M");

var_dump(gzfile($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/oom_gzfile.gz");
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
