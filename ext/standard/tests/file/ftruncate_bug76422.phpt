--TEST--
Bug #76422 ftruncate fails on files > 2GB
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die('skip.. only valid for 64-bit');
}
if (disk_free_space(__DIR__) <= 4.1 * 1024 * 1024 * 1024 ) {
    // Add a bit of extra overhead for other processes, temporary files created while running tests, etc.
    die('skip.. This test requires over 4GB of free disk space on this disk partition');
}
?>
--FILE--
<?php

$fn = __DIR__ . DIRECTORY_SEPARATOR . "test76422";

$file_handle = fopen($fn,'cb');

if (false === $file_handle) {
    die('Cannot open test file :/');
}

/* Check if ftruncate() with 2GB works. If it doesn't, it's likely that large files are
 * generally not supported (EFBIG). */
$truncate_offset = 2 * 1024 * 1024 * 1024;
$ftruncate_result = ftruncate($file_handle, $truncate_offset);
if (false === $ftruncate_result) {
    var_dump(true);
    fclose($file_handle);
    unlink($fn);
    return;
}

$truncate_offset = 4 * 1024 * 1024 * 1024 + 1;
$ftruncate_result = ftruncate($file_handle, $truncate_offset);

if (false === $ftruncate_result) {
    // NOTE: unlink() is deliberately repeated - If this test runs out of disk space attempting to reserve space for this temporary file,
    // then the--CLEAN-- script can't be run (if we don't delete the file),
    // because there wouldn't be any free disk space to save a new php file.
    fclose($file_handle);
    unlink($fn);
    die('Truncate has failed :/');
}

fclose($file_handle);
var_dump(filesize($fn) >= $truncate_offset);
unlink($fn);
?>
--CLEAN--
<?php
$fn = __DIR__ . "/test76422";
@unlink($fn);
?>
--EXPECT--
bool(true)
