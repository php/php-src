<?php
/**
 * Test script for finfo_file() with remote URLs
 * This tests the fix for the bug where finfo_file() returned false for remote URLs
 */

echo "Testing finfo_file() with remote URLs\n";
echo str_repeat("=", 50) . "\n\n";

$finfo = finfo_open();

// Check if HTTPS wrapper is available
$wrappers = stream_get_wrappers();
$hasHttps = in_array('https', $wrappers);

// Test 1: Remote URL (the original failing case)
echo "Test 1: Remote URL (https://example.com)\n";
if (!$hasHttps) {
    echo "⚠ SKIPPED: HTTPS wrapper not available (OpenSSL support required)\n";
    echo "   To enable HTTPS support, rebuild PHP with: ./configure --with-openssl\n\n";
} else {
    $result = finfo_file($finfo, "https://example.com");
    if ($result === false) {
        echo "❌ FAILED: finfo_file() returned false (this is the bug we're fixing)\n";
    } else {
        echo "✅ PASSED: finfo_file() returned: " . var_export($result, true) . "\n";
        // Expected something like: "HTML document, ASCII text, with very long lines (512)"
        if (stripos($result, "HTML") !== false || stripos($result, "text") !== false) {
            echo "   ✓ Result contains expected content type indicators\n";
        }
    }
}
echo "\n";

// Test 2: Local file (should still work)
echo "Test 2: Local file (this script itself)\n";
$result = finfo_file($finfo, __FILE__);
if ($result === false) {
    echo "❌ FAILED: finfo_file() returned false for local file\n";
} else {
    echo "✅ PASSED: finfo_file() returned: " . var_export($result, true) . "\n";
}
echo "\n";

// Test 3: Directory (should still work and return "directory")
echo "Test 3: Directory\n";
$result = finfo_file($finfo, __DIR__);
if ($result === false) {
    echo "❌ FAILED: finfo_file() returned false for directory\n";
} else {
    echo "✅ PASSED: finfo_file() returned: " . var_export($result, true) . "\n";
    if ($result === "directory") {
        echo "   ✓ Correctly identified as directory\n";
    } else {
        echo "   ⚠ WARNING: Expected 'directory', got something else\n";
    }
}
echo "\n";

// Test 4: Another remote URL (different domain)
echo "Test 4: Another remote URL (http://httpbin.org/html)\n";
$result = finfo_file($finfo, "http://httpbin.org/html");
if ($result === false) {
    echo "❌ FAILED: finfo_file() returned false for remote URL\n";
} else {
    echo "✅ PASSED: finfo_file() returned: " . var_export($result, true) . "\n";
}
echo "\n";

echo str_repeat("=", 50) . "\n";
echo "Test completed!\n";

