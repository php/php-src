--TEST--
GH-16979: Test opcache_is_script_cached_in_file_cache function
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh16979_cache', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh16979_cache"
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php

$file = __DIR__ . '/gh16979_check_file_cache_function.inc';
var_dump(opcache_is_script_cached_in_file_cache($file));
opcache_compile_file($file);
var_dump(opcache_is_script_cached_in_file_cache($file));
opcache_invalidate($file, force: true);
var_dump(opcache_is_script_cached_in_file_cache($file));

?>
--CLEAN--
<?php
function removeDirRecursive($dir) {
    if (!is_dir($dir)) return;
    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($iterator as $fileinfo) {
        if ($fileinfo->isDir()) {
            @rmdir($fileinfo->getRealPath());
        } else {
            @unlink($fileinfo->getRealPath());
        }
    }
    @rmdir($dir);
}
removeDirRecursive(__DIR__ . '/gh16979_cache');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
