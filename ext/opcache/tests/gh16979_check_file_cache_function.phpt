--TEST--
GH-16979: Test opcache_is_script_cached_in_file_cache function
--SKIPIF--
<?php
// Ensure the cache directory exists BEFORE OPcache needs it
$cacheDir = __DIR__ . '/gh16979_cache';
if (!is_dir($cacheDir)) {
    @mkdir($cacheDir, 0777, true);
}
// Check if mkdir failed potentially due to permissions
if (!is_dir($cacheDir) || !is_writable($cacheDir)) {
    die('skip Could not create or write to cache directory: ' . $cacheDir);
}
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh16979_cache"
--EXTENSIONS--
opcache
--FILE--
<?php

opcache_compile_file(__FILE__);

var_dump(opcache_is_script_cached_in_file_cache(__FILE__));

opcache_invalidate(__FILE__, true); // force=true

var_dump(opcache_is_script_cached_in_file_cache(__FILE__));

?>
--CLEAN--
<?php
require __DIR__ . '/cleanup_helper.inc';

$baseCacheDir = __DIR__ . '/gh16979_cache';

removeDirRecursive($baseCacheDir);
?>
--EXPECTF--
bool(true)
bool(false)
