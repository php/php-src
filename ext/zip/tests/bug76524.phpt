--TEST--
ZipArchive Bug #76524 (memory leak with ZipArchive::OVERWRITE flag and empty archive)
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$filename = __DIR__ . '/nonexistent.zip';

// The error is not reproduced when file already exist:
if (!file_exists($filename)) {
	$zip = new ZipArchive();
	$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
	echo 'ok';
} else {
	echo "file $filename exists, something goes wrong";
}

?>
--EXPECTF--
ok
Warning: Unknown: Cannot destroy the zip context in Unknown on line 0
