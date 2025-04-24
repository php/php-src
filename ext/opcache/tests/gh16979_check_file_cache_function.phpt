--TEST--
GH-16979: Test opcache_is_script_cached_in_file_cache function
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available');

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
opcache.jit_buffer_size=0
opcache.file_cache="{PWD}/gh16979_cache"
opcache.file_cache_only=0
opcache.validate_timestamps=0
--EXTENSIONS--
opcache
--FILE--
<?php
$file = __FILE__; // Test using the test file itself
$cacheDir = __DIR__ . '/gh16979_cache';

// --- Initial checks ---
echo "Initial state:\n";
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

// --- Populate ---
echo "\nPopulating cache via opcache_compile_file():\n";
opcache_compile_file($file);
echo "\nState after compile:\n";
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

// --- Invalidate SHM (and implicitly file cache according to code) ---
echo "\nInvalidating SHM only (force=false):\n";
// NOTE: Underlying zend_accel_invalidate calls zend_file_cache_invalidate
// regardless of force=false if opcache.file_cache is set and opcache.file_cache_only=0.
// This affects the file cache status check below.
opcache_invalidate($file); // force=false is default

echo "\nState after SHM invalidate:\n";
var_dump(opcache_is_script_cached($file)); // SHM invalidated
var_dump(opcache_is_script_cached_in_file_cache($file)); // File cache status also affected

// --- Force Invalidate (should definitely clear both) ---
// Re-populate first to have something to invalidate fully
opcache_compile_file($file);
echo "\nRe-populated state:\n";
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

echo "\nForcing file cache invalidation (force=true):\n";
opcache_invalidate($file, true); // force=true

echo "\nState after file cache invalidate:\n";
var_dump(opcache_is_script_cached($file));
var_dump(opcache_is_script_cached_in_file_cache($file));

// --- File system check ---
echo "\nChecking file system:\n";
if (substr(PHP_OS, 0, 3) === 'WIN') {
    $sanitizedDir = str_replace(':', '', __DIR__);
    // Need to correctly form the expected path fragment based on default OPcache hashing
    // This usually involves system ID and path mangling. Let's assume 2 levels for pattern.
    $pattern = $cacheDir . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . $sanitizedDir . DIRECTORY_SEPARATOR . basename(__FILE__) . '.bin';
} else {
    $pattern = $cacheDir . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . __DIR__ . DIRECTORY_SEPARATOR . basename(__FILE__) . '.bin';
}
$found = glob($pattern);
// Expect file to be gone after invalidate(true)
var_dump(count($found) === 0);

?>
--CLEAN--
<?php
$baseCacheDir = __DIR__ . '/gh16979_cache';

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

Populating cache via opcache_compile_file():

State after compile:
bool(true)
bool(true)

Invalidating SHM only (force=false):

State after SHM invalidate:
bool(false)
bool(false)

Re-populated state:
bool(true)
bool(true)

Forcing file cache invalidation (force=true):

State after file cache invalidate:
bool(false)
bool(false)

Checking file system:
bool(true)
