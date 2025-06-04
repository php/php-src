--TEST--
GH-16551: Verify opcache_invalidate fails or is ineffective when opcache.file_cache_only=1
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available');
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip Test relies on shell_exec and specific helper script pathing, skipping on Windows');
$php_binary = getenv('TEST_PHP_EXECUTABLE');
if (!$php_binary) die('skip TEST_PHP_EXECUTABLE environment variable not set');
if (!is_executable($php_binary)) die("skip $php_binary is not executable");

// Ensure the cache directory exists BEFORE OPcache needs it
$cacheDir = __DIR__ . '/gh16551_invalidate_fco_cache';
if (!is_dir($cacheDir)) {
    @mkdir($cacheDir, 0777, true);
}
// Check if mkdir failed potentially due to permissions
if (!is_dir($cacheDir) || !is_writable($cacheDir)) {
    die('skip Could not create or write to cache directory: ' . $cacheDir);
}
?>
--INI--
; Main test runs with file_cache_only=1
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_cache="{PWD}/gh16551_invalidate_fco_cache"
opcache.file_cache_only=1
opcache.validate_timestamps=0
--EXTENSIONS--
opcache
--FILE--
<?php
$phpBinary = getenv('TEST_PHP_EXECUTABLE');
$helperScript = __DIR__ . '/gh16551_populate_cache_helper.inc';
$fileToCache = __DIR__ . '/gh16551_998.inc';
$cacheDir = __DIR__ . '/gh16551_invalidate_fco_cache';
$helperOutputFile = $cacheDir . '/helper_output.txt';

// 1. Populate cache using helper (runs WITHOUT file_cache_only=1)
$opcache_extension = (PHP_SHLIB_SUFFIX === 'dll') ? 'php_opcache.dll' : 'opcache.so';
$cmd = escapeshellarg($phpBinary) . ' ' .
       '-n ' . // <-- Tell PHP *not* to load any php.ini files
       '-d zend_extension=' . escapeshellarg($opcache_extension) . ' '. // <-- Explicitly load OPcache
       '-d opcache.enable=1 -d opcache.enable_cli=1 ' .
       '-d opcache.file_cache=' . escapeshellarg($cacheDir) . ' ' .
       '-d opcache.file_cache_only=0 ' . // <-- Helper runs normally
       '-d opcache.validate_timestamps=1 ' .
       '-d opcache.jit_buffer_size=0 ' . // Ensure helper doesn't try JIT
       escapeshellarg($helperScript) . ' ' .
       escapeshellarg($fileToCache) . ' ' .
       escapeshellarg($cacheDir) . ' ' .                             // Pass path for verification/output
       escapeshellarg($helperOutputFile);

echo "Running helper script to populate cache...\n";
$helperResult = shell_exec($cmd); // Execute the command

// 3. Check if helper script succeeded via output file
if (!file_exists($helperOutputFile) || trim(file_get_contents($helperOutputFile)) !== 'SUCCESS') {
    echo "Helper script failed:\n";
    if (file_exists($helperOutputFile)) {
        readfile($helperOutputFile); // Show helper error message
    } else {
        echo "Helper output file '$helperOutputFile' not found.";
    }
    echo "\nShell Exec Output: " . $helperResult;
    exit(1); // Abort test
}
echo "Helper script successful.\n";

// 2. Verify cache exists in main process (running file_cache_only=1)
echo "\nVerifying initial state (file_cache_only=1, cache populated):\n";
var_dump(opcache_is_script_cached($fileToCache)); // Should be false (SHM not used)
var_dump(opcache_is_script_cached_in_file_cache($fileToCache)); // Should be true

// 3. Attempt to invalidate (should fail or do nothing due to file_cache_only=1)
echo "\nAttempting opcache_invalidate() with file_cache_only=1:\n";
$invalidate_result = opcache_invalidate($fileToCache, true); // force=true
var_dump($invalidate_result); // Expect bool(false) as the function exits early

// 4. Verify cache state *after* invalidate attempt (should be unchanged)
echo "\nVerifying state after invalidate attempt:\n";
var_dump(opcache_is_script_cached($fileToCache)); // Still false
var_dump(opcache_is_script_cached_in_file_cache($fileToCache)); // Should STILL be true

?>
--CLEAN--
<?php
$baseCacheDir = __DIR__ . '/gh16551_invalidate_fco_cache';

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
Running helper script to populate cache...
Helper script successful.

Verifying initial state (file_cache_only=1, cache populated):
bool(false)
bool(true)

Attempting opcache_invalidate() with file_cache_only=1:
bool(false)

Verifying state after invalidate attempt:
bool(false)
bool(true)