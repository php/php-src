--TEST--
GH-16551: Populate memory and file cache, check status
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available');

// Ensure the cache directory exists BEFORE OPcache needs it
$cacheDir = __DIR__ . '/gh16551_populate_cache';
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
opcache.jit_buffer_size=0
opcache.file_cache="{PWD}/gh16551_populate_cache"
opcache.file_cache_only=0
opcache.validate_timestamps=1
--EXTENSIONS--
opcache
--FILE--
<?php
$file = __DIR__ . '/gh16551_998.inc';
$cacheDir = __DIR__ . '/gh16551_populate_cache';

echo "Initial state:\n";
// Initial state might be true if run during Pass 2 of OPCACHE_VARIATION
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

echo "\nPopulating cache...\n";
// Compile always attempts to populate both SHM and file cache here
opcache_compile_file($file);

echo "\nState after opcache_compile_file():\n";
// After compile in this run, state should be true for both
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

echo "\nRunning file via require:\n";
require $file; // Outputs 9

echo "\nState after require:\n";
// State should remain true
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));
?>
--CLEAN--
<?php
$baseCacheDir = __DIR__ . '/gh16551_populate_cache';

function removeDirRecursive($dir) {
    if (!is_dir($dir)) return;
    try {
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
    } catch (UnexpectedValueException $e) { @rmdir($dir); } catch (Exception $e) { @rmdir($dir); }
}

removeDirRecursive($baseCacheDir);
?>
--EXPECTF--
Initial state:
bool(%s)
bool(%s)

Populating cache...

State after opcache_compile_file():
bool(true)
bool(true)

Running file via require:
9

State after require:
bool(true)
bool(true)
