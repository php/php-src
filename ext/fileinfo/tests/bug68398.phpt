--TEST--
Bug #68398: msooxml matches too many archives
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$f = new finfo(FILEINFO_MIME);
var_dump($f->file(dirname(__FILE__) . DIRECTORY_SEPARATOR . '68398.zip'));
?>
+++DONE+++
--EXPECT--
string(31) "application/zip; charset=binary"
+++DONE+++
