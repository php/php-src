--TEST--
GH-16551: Verify reads work with opcache.file_cache_read_only=1 from pre-populated cache
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available');
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip Test relies on shell_exec and specific helper script pathing, skipping on Windows');
$php_binary = getenv('TEST_PHP_EXECUTABLE');
if (!$php_binary) die('skip TEST_PHP_EXECUTABLE environment variable not set');
if (!is_executable($php_binary)) die("skip $php_binary is not executable");

// Ensure the cache directory exists BEFORE OPcache needs it
$cacheDir = __DIR__ . '/gh16551_readonly_read_cache';
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
opcache.file_cache="{PWD}/gh16551_readonly_read_cache"
opcache.file_cache_read_only=1
opcache.validate_timestamps=1
--EXTENSIONS--
opcache
--FILE--
<?php
$phpBinary = getenv('TEST_PHP_EXECUTABLE');
$helperScript = __DIR__ . '/gh16551_populate_cache_helper.inc';
$fileToCache = __DIR__ . '/gh16551_998.inc';
$cacheDir = __DIR__ . '/gh16551_readonly_read_cache';
$helperOutputFile = $cacheDir . '/helper_output.txt';

// 1. Ensure cache directory exists for helper output & cache itself
@mkdir($cacheDir, 0777, true);

// 2. Prepare and run the helper script to populate the cache
// Pass necessary INI settings via -d for the helper process
$opcache_extension = (PHP_SHLIB_SUFFIX === 'dll') ? 'php_opcache.dll' : 'opcache.so';
$cmd = escapeshellarg($phpBinary) . ' ' .
       '-n ' . // <-- Tell PHP *not* to load any php.ini files
       '-d zend_extension=' . escapeshellarg($opcache_extension) . ' '. // <-- Explicitly load OPcache
       '-d opcache.enable=1 ' .
       '-d opcache.enable_cli=1 ' .
       '-d opcache.file_cache=' . escapeshellarg($cacheDir) . ' ' .  // CRITICAL: Tell helper where to cache
       '-d opcache.file_cache_read_only=0 ' .                        // CRITICAL: Allow helper to write
       '-d opcache.validate_timestamps=1 ' .                         // Consistent setting
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

// 4. Now, in the main test process (with read_only=1 from --INI--), attempt to use the cached file
echo "\nInitial state before read attempt (read-only mode, cache populated by helper):\n";
// SHM should be false initially in this process
var_dump(opcache_is_script_cached($fileToCache));
// File cache *should* now be true due to helper script
var_dump(opcache_is_script_cached_in_file_cache($fileToCache));

echo "\nAttempting require of pre-cached file in read-only mode:\n";
require $fileToCache; // Outputs 9 - This MUST succeed by reading file cache

echo "\nState after require (read-only mode):\n";
// File cache should remain true. SHM state after execution from file cache might vary.
var_dump(opcache_is_script_cached($fileToCache));
var_dump(opcache_is_script_cached_in_file_cache($fileToCache));

?>
--CLEAN--
<?php
$baseCacheDir = __DIR__ . '/gh16551_readonly_read_cache';

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

Initial state before read attempt (read-only mode, cache populated by helper):
bool(false)
bool(true)

Attempting require of pre-cached file in read-only mode:
9

State after require (read-only mode):
bool(%s)
bool(true)
