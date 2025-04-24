--TEST--
GH-16551: Behavior with opcache.file_cache_only=1
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available');

// Ensure the cache directory exists BEFORE OPcache needs it
$cacheDir = __DIR__ . '/gh16551_fileonly_cache';
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
opcache.file_cache="{PWD}/gh16551_fileonly_cache"
opcache.file_cache_only=1
opcache.validate_timestamps=1
--EXTENSIONS--
opcache
--FILE--
<?php
$file = __DIR__ . '/gh16551_998.inc';
$uncached_file = __DIR__ . '/gh16551_999.inc';
$cacheDir = __DIR__ . '/gh16551_fileonly_cache';

echo "Initial state (file_cache_only mode):\n";
// SHM is always false, File Cache might be true in Pass 2
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

echo "\nAttempting opcache_compile_file():\n";
opcache_compile_file($file);

echo "\nState after compile attempt:\n";
// SHM remains false, File Cache becomes true
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

// Check file existence via glob
echo "\nChecking file system for compiled file:\n";
if (substr(PHP_OS, 0, 3) === 'WIN') {
    $sanitizedDir = str_replace(':', '', __DIR__);
    $pattern = $cacheDir . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . $sanitizedDir . DIRECTORY_SEPARATOR . 'gh16551_998.inc.bin';
} else {
    $pattern = $cacheDir . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . __DIR__ . DIRECTORY_SEPARATOR . 'gh16551_998.inc.bin';
}
$found = glob($pattern);
var_dump(count($found) > 0); // Expect true after compile

echo "\nAttempting require:\n";
require $file; // Outputs 9, should execute from file cache

echo "\nState after require:\n";
// State remains unchanged
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

echo "\nChecking uncached file initial state:\n";
// SHM false, File Cache might be true in Pass 2 for this file too
var_dump(opcache_is_script_cached($uncached_file));
var_dump(opcache_is_script_cached_in_file_cache($uncached_file));

echo "\nRequiring uncached file:\n";
require $uncached_file; // Outputs 8, should compile to file cache now

echo "\nState after requiring uncached file:\n";
// SHM remains false, File cache becomes true for this file
var_dump(opcache_is_script_cached($uncached_file));
var_dump(opcache_is_script_cached_in_file_cache($uncached_file));

?>
--CLEAN--
<?php
$baseCacheDir = __DIR__ . '/gh16551_fileonly_cache';

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
Initial state (file_cache_only mode):
bool(false)
bool(%s)

Attempting opcache_compile_file():

State after compile attempt:
bool(false)
bool(true)

Checking file system for compiled file:
bool(true)

Attempting require:
9

State after require:
bool(false)
bool(true)

Checking uncached file initial state:
bool(false)
bool(%s)

Requiring uncached file:
8

State after requiring uncached file:
bool(false)
bool(true)
