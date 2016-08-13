--TEST--
Bug #69320 libmagic crash when running laravel tests
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$fname = dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug69320.txt";
file_put_contents($fname, "foo");
var_dump(finfo_file(finfo_open(FILEINFO_MIME_TYPE), $fname));

?>
--CLEAN--
<?php
	$fname = dirname(__FILE__) . DIRECTORY_SEPARATOR . "bug69320.txt";
	unlink($fname);
?>
--EXPECTF--
string(10) "text/plain"

