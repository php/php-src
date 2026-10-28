--TEST--
Native ZendMM ASan allocation lifetime annotations
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_asan_zend_mm')) {
    die('skip requires AddressSanitizer');
}
?>
--FILE--
<?php
$sizes = [1, 16, 128, 3072, 4096, 16384, 3 * 1024 * 1024];
foreach ($sizes as $oldSize) {
    foreach ($sizes as $newSize) {
        foreach (zend_test_asan_zend_mm($oldSize, $newSize) as $check => $passed) {
            if (!$passed) {
                echo "$oldSize -> $newSize: $check failed\n";
            }
        }
    }
}
echo "Done\n";
?>
--EXPECT--
Done
