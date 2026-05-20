--TEST--
GH-9812: memory usage grows on repeated include with opcache.file_cache_only=1
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh9812_cache', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh9812_cache"
opcache.file_cache_only=1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php

$file = __DIR__ . '/gh9812.inc';

// Warm-up: write the script into the file cache
require $file;
require $file;

$baseline = memory_get_usage();
for ($i = 0; $i < 200; $i++) {
    require $file;
}
$delta = memory_get_usage() - $baseline;

// Allow a small headroom for VM/JIT.
echo $delta < 4096 ? "stable\n" : "leaking: $delta bytes\n";

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
removeDirRecursive(__DIR__ . '/gh9812_cache');
?>
--EXPECT--
stable
