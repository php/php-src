--TEST--
Bug #50678 files extracted by ZipArchive class lost their original modified time
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip zip extension not available');
?>
--FILE--
<?php
$dirname = __DIR__ . '/bug50678';
$zipname = $dirname . '/bug50678.zip';
$file    = __DIR__ . '/utils.inc';

@mkdir($dirname);

// Archive a file
$zip = new ZipArchive();
$zip->open($zipname, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFile($file, 'foo');
$zip->close();

// Extract it
$zip->open($zipname);
$zip->extractTo($dirname);
$zip->close();

// Time is preserved
var_dump(filemtime($file) == filemtime($dirname . '/foo'));
?>
Done
--EXPECT--
bool(true)
Done
--CLEAN--
<?php
include __DIR__ . '/utils.inc';
rmdir_rf(__DIR__ . '/bug50678');
?>
