--TEST--
Test FILEINFO_EXTENSION flag
--SKIPIF--
<?php
if (!class_exists('finfo'))
	die('skip no fileinfo extension');
--FILE--
<?php

$f = new finfo;
var_dump($f->file(dirname(__FILE__) . "/resources/test.jpg", FILEINFO_EXTENSION));

?>
===DONE===
--EXPECT--
string(17) "jpeg/jpg/jpe/jfif"
===DONE===
