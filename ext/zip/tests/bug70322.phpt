--TEST--
Bug #70322 (ZipArchive::close() doesn't indicate errors)
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip requires zip extension');
?>
--FILE--
<?php
$zip = new ZipArchive();
var_dump($zip->open(__DIR__ . '/bug70322.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE));
var_dump($zip->close());
?>
--CLEAN--
<?php
// we don't expect the archive to be created, but clean up just in case...
@unlink(__DIR__ . '/bug70322.zip');
?>
--EXPECTF--
bool(true)

Warning: ZipArchive::close(): Can't remove file: No such file or directory in %s%ebug70322.php on line %d
bool(false)
