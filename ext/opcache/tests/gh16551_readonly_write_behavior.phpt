--TEST--
GH-16551: Verify writes are blocked by opcache.file_cache_read_only=1
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available');

// Ensure the cache directory exists BEFORE OPcache needs it
$cacheDir = __DIR__ . '/gh16551_readonly_write_cache';
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
opcache.file_cache="{PWD}/gh16551_readonly_write_cache"
opcache.file_cache_read_only=1
opcache.validate_timestamps=1
opcache.file_cache_consistency_checks=1
--EXTENSIONS--
opcache
--FILE--
<?php
$file_to_compile = __DIR__ . '/gh16551_998.inc';
$file_to_require = __DIR__ . '/gh16551_999.inc';
$cacheDir = __DIR__ . '/gh16551_readonly_write_cache';

echo "Initial state (read-only mode):\n";
// Expect false as cache dir is clean and read_only=1 prevents writes
var_dump(opcache_is_script_cached($file_to_compile));
var_dump(opcache_is_script_cached_in_file_cache($file_to_compile));
var_dump(opcache_is_script_cached($file_to_require));
var_dump(opcache_is_script_cached_in_file_cache($file_to_require));

echo "\nAttempting opcache_compile_file() in read-only mode:\n";
opcache_compile_file($file_to_compile);

echo "\nState after compile attempt:\n";
// SHM gets populated, File cache does not
var_dump(opcache_is_script_cached($file_to_compile));
var_dump(opcache_is_script_cached_in_file_cache($file_to_compile));

echo "\nAttempting require in read-only mode:\n";
require $file_to_require; // Outputs 8

echo "\nState after require attempt:\n";
// SHM gets populated, File cache does not
var_dump(opcache_is_script_cached($file_to_require));
var_dump(opcache_is_script_cached_in_file_cache($file_to_require));

echo "\nAttempting opcache_invalidate() on SHM-cached script in read-only mode:\n";
// Invalidation of SHM should succeed
var_dump(opcache_invalidate($file_to_compile, true)); // force=true still respects read_only for file cache

echo "\nState after invalidate attempt:\n";
// SHM is now false, file cache remains false (was never populated, invalidate ignored file cache)
var_dump(opcache_is_script_cached($file_to_compile));
var_dump(opcache_is_script_cached_in_file_cache($file_to_compile));

?>
--CLEAN--
<?php
$baseCacheDir = __DIR__ . '/gh16551_readonly_write_cache';

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
--EXPECT--
Initial state (read-only mode):
bool(false)
bool(false)
bool(false)
bool(false)

Attempting opcache_compile_file() in read-only mode:

State after compile attempt:
bool(true)
bool(false)

Attempting require in read-only mode:
8

State after require attempt:
bool(true)
bool(false)

Attempting opcache_invalidate() on SHM-cached script in read-only mode:
bool(true)

State after invalidate attempt:
bool(false)
bool(false)
