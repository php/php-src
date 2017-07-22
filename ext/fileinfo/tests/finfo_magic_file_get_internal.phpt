--TEST--
Test the internal magic file path
--SKIPIF--
<?php
if (!class_exists('finfo'))
	die('skip no fileinfo extension');
--FILE--
<?php

$finfo = new finfo();
var_dump($finfo->getMagicFile());

$finfo = finfo_open();
var_dump(finfo_magic_file_get($finfo));

?>
===DONE===
--EXPECT--
string(0) ""
string(0) ""
===DONE===

