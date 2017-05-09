--TEST--
Bug #50678 (files extracted by ZipArchive class loose their original modified time)
--SKIPIF--
<?php
if (!extension_loaded('zip')) print 'skip zip extension not available';
?>
--FILE--
<?php
$dest = __DIR__ . '/bug50678';
mkdir($dest);
$zip = new ZipArchive();
$zip->open(__DIR__ . '/bug50678.zip');
$zip->extractTo($dest);
$zip->close();
$filename = $dest . '/bug50678.txt';
// check that the mtime is properly set, if the extracted file is writable
var_dump(!is_writable($filename) || filemtime($filename) == 1432163274);
?>
--CLEAN--
<?php
$dest = __DIR__ . '/bug50678';
unlink($dest . '/bug50678.txt');
rmdir($dest);
?>
--EXPECT--
bool(true)
