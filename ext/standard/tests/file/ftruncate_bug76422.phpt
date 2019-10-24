--TEST--
Bug #76422 ftruncate fails on files > 2GB
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die('skip.. only valid for 64-bit');
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
    return;
}

$truncate_offset = 4 * 1024 * 1024 * 1024 + 1;
$ftruncate_result = ftruncate($file_handle, $truncate_offset);

if (false === $ftruncate_result) {
    die('Truncate has failed :/');
}

fclose($file_handle);
var_dump(filesize($fn) >= $truncate_offset);
?>
--CLEAN--
<?php
$fn = __DIR__ . "/test76422";
unlink($fn);
?>
--EXPECT--
bool(true)
