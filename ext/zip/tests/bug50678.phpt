--TEST--
Bug #50678 (files extracted by ZipArchive class loose their original modified time)
--FILE--
<?php
$dest = __DIR__ . '/bug50678';
mkdir($dest);
$zip = new ZipArchive();
$zip->open(__DIR__ . '/bug50678.zip');
$zip->extractTo($dest);
$zip->close();
var_dump(filemtime($dest . '/bug50678.txt'));
?>
--CLEAN--
<?php
$dest = __DIR__ . '/bug50678';
unlink($dest . '/bug50678.txt');
rmdir($dest);
?>
--EXPECT--
int(1432163274)
