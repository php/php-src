--TEST--
Test the external magic file path
--SKIPIF--
<?php
if (!class_exists('finfo'))
	die('skip no fileinfo extension');
--FILE--
<?php

$finfo = new finfo(FILEINFO_NONE,  __DIR__ . DIRECTORY_SEPARATOR . 'magic');
$path = $finfo->getMagicFile();
var_dump(substr($path, strlen(dirname(__DIR__, 3))));

$finfo = finfo_open(FILEINFO_NONE,  __DIR__ . DIRECTORY_SEPARATOR . 'magic');
$path = finfo_magic_file_get($finfo);
var_dump(substr($path, strlen(dirname(__DIR__, 3))));

?>
===DONE===
--EXPECTF--
string(25) "%eext%efileinfo%etests%emagic"
string(25) "%eext%efileinfo%etests%emagic"
===DONE===

