--TEST--
Bug #68735 fileinfo out-of-bounds memory access
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
	$test_file = dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug68735.jpg";
	$f = new finfo;

	var_dump($f->file($test_file));

?>
===DONE===
--EXPECTF--
string(%d) "JPEG image data, JFIF standard 1.01, comment: "%S""
===DONE===
