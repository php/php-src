<?php

// Helper script to populate opcache file cache
// Expected INI settings (like opcache.enable, opcache.file_cache, opcache.file_cache_read_only=0)
// MUST be set via -d flags by the calling process.

if ($argc < 4) {
    echo "Usage: php gh16551_populate_cache_helper.inc <file_to_cache> <cache_dir_path> <output_file>\n";
    exit(1);
}

$fileToCache = $argv[1];
$cacheDirPath = $argv[2]; // Path for verification and output file
$outputFile = $argv[3];   // File to signal completion/success

// Basic validation
if (!file_exists($fileToCache)) {
    @file_put_contents($outputFile, "ERROR: File to cache '$fileToCache' not found.");
    exit(1);
}
// Ensure base directory exists, primarily for the output file placement
if (!is_dir($cacheDirPath) && !@mkdir($cacheDirPath, 0777, true)) {
     @file_put_contents($outputFile, "ERROR: Could not create cache directory '$cacheDirPath' for output.");
     // Don't necessarily exit, opcache might still create it, but log potential issue.
     // Proceed with caution.
}

// Check if OPcache extension is loaded (essential)
if (!extension_loaded('Zend OPcache')) {
     @file_put_contents($outputFile, "ERROR: Zend OPcache extension not loaded in helper script.");
     exit(1);
}

// Compile the file to populate the cache (relies on -d flags for correct behavior)
$compileSuccess = opcache_compile_file($fileToCache);

if (!$compileSuccess) {
    @file_put_contents($outputFile, "ERROR: opcache_compile_file failed for '$fileToCache'. Check -d INI settings.");
    exit(1);
}

// Verification step: Check if the file cache was actually created
// This confirms the INI settings allowed writing to the expected location.
if (substr(PHP_OS, 0, 3) === 'WIN') {
    // Use dirname() on the *target* file path, sanitize drive letter
    $sanitizedDirName = str_replace(':', '', dirname($fileToCache));
    $pattern = $cacheDirPath . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . $sanitizedDirName . DIRECTORY_SEPARATOR . basename($fileToCache) . '.bin';
} else {
    // Use dirname() on the *target* file path
    $pattern = $cacheDirPath . DIRECTORY_SEPARATOR . '*' . DIRECTORY_SEPARATOR . dirname($fileToCache) . DIRECTORY_SEPARATOR . basename($fileToCache) . '.bin';
}

// Use glob with a small delay/retry for filesystem eventual consistency
$tries = 3;
$found = [];
while ($tries > 0) {
    clearstatcache(); // Clear filesystem stat cache
    $found = glob($pattern);
    if (!empty($found)) {
        break;
    }
    $tries--;
    if ($tries > 0) {
        usleep(100000); // Wait 100ms before retrying glob
    }
}

if (!empty($found)) {
    @file_put_contents($outputFile, "SUCCESS");
    exit(0);
} else {
    @file_put_contents($outputFile, "ERROR: opcache_compile_file succeeded but cache file pattern not found after retries: $pattern");
    exit(1);
}
