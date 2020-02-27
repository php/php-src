--TEST--
ziparchive::count()
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = __DIR__ . '/';
$file = $dirname . 'test.zip';

$zip = new ZipArchive;
if (!$zip->open($file)) {
    exit('failed');
}

var_dump($zip->numFiles, count($zip), $zip->numFiles == count($zip));
?>
Done
--EXPECT--
int(4)
int(4)
bool(true)
Done
