--TEST--
Bug #70322 (ZipArchive::close() doesn't indicate errors)
--DESCRIPTION--
We want to test whether ZipArchive::close() returns FALSE and raises a warning
on failure, so we force the failure by adding a file to the archive, which we
delete before closing.
--EXTENSIONS--
zip
--FILE--
<?php
$zipfile = __DIR__ . '/bug70322.zip';
$textfile = __DIR__ . '/bug70322.txt';
touch($textfile);
$zip = new ZipArchive();
$zip->open($zipfile, ZipArchive::CREATE);
$zip->addFile($textfile);
unlink($textfile);
var_dump($zip->close());
?>
--CLEAN--
<?php
// we don't expect the archive to be created, but clean up just in case...
@unlink(__DIR__ . '/bug70322.zip');
?>
--EXPECTF--
Warning: ZipArchive::close(): %s: No such file or directory in %s%ebug70322.php on line %d
bool(false)
