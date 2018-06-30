--TEST--
ZipArchive Bug #76524 (memory leak with ZipArchive::OVERWRITE flag and empty archive)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip');
?>
--FILE--
<?php

$filename = __DIR__ . '/nonexistent.zip';

$zip = new ZipArchive();
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
echo 'ok';
?>
--EXPECTF--
ok
