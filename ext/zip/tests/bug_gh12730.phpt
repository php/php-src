--TEST--
Bug GH-12730 - extract lost permission on linux
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (version_compare(ZipArchive::LIBZIP_VERSION, '0.11.2', '<')) die('skip libzip < 0.11.2');
?>
--FILE--
<?php
include __DIR__ . '/utils.inc';


$zip = new ZipArchive();
$zip->open(__DIR__ . '/bug_gh12730.zip');
$zip->extractTo(__DIR__);
$zip->close();

var_dump(fileperms(__DIR__ . '/bug_gh12730/foo')); // file
var_dump(fileperms(__DIR__ . '/bug_gh12730/bar')); // executable
var_dump(fileperms(__DIR__ . '/bug_gh12730')    ); // dir
?>
Done
--CLEAN--
<?php
unlink(__DIR__ . '/bug_gh12730/foo');
unlink(__DIR__ . '/bug_gh12730/bar');
rmdir(__DIR__ . '/bug_gh12730');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
Done
