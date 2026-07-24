--TEST--
GH-17770: opcache.file_cache writes with JIT enabled
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh17770_cache', 0777, true);
if (ini_get('opcache.jit') === false) {
    die('skip JIT is not supported');
}
?>
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.file_cache="{PWD}/gh17770_cache"
opcache.file_cache_consistency_checks=0
--FILE--
<?php
function removeDirRecursive(string $dir): void {
    if (!is_dir($dir)) {
        return;
    }

    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );

    foreach ($iterator as $entry) {
        if ($entry->isDir()) {
            @rmdir($entry->getRealPath());
        } else {
            @unlink($entry->getRealPath());
        }
    }

    @rmdir($dir);
}

$cacheDir = ini_get('opcache.file_cache');
removeDirRecursive($cacheDir);
@mkdir($cacheDir, 0777, true);

$tmpPhpFile = __DIR__ . '/gh17770-opcache-target.php';
file_put_contents($tmpPhpFile, "<?php\nfunction gh17770() { return 42; }\ngh17770();\n");

@opcache_reset();
opcache_compile_file($tmpPhpFile);

$hasCacheFile = opcache_is_script_cached_in_file_cache($tmpPhpFile);

echo $hasCacheFile ? "OK\n" : "FAIL: opcache file cache is empty\n";
?>
--CLEAN--
<?php
function removeDirRecursive(string $dir): void {
    if (!is_dir($dir)) {
        return;
    }

    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );

    foreach ($iterator as $entry) {
        if ($entry->isDir()) {
            @rmdir($entry->getRealPath());
        } else {
            @unlink($entry->getRealPath());
        }
    }

    @rmdir($dir);
}

removeDirRecursive(__DIR__ . '/gh17770_cache');
@unlink(__DIR__ . '/gh17770-opcache-target.php');
?>
--EXPECT--
OK
