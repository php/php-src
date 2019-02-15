--TEST--
Bug #67516 wrong mimetypes with finfo_file(filename, FILEINFO_MIME_TYPE)
--SKIPIF--
<?php
if (!class_exists('finfo'))
	die('skip no fileinfo extension');
--FILE--
<?php

$f = new finfo;
var_dump($f->file(dirname(__FILE__) . "/bug67516.gif", FILEINFO_MIME_TYPE));
var_dump($f->file(dirname(__FILE__) . "/bug67516.gif", FILEINFO_MIME));
?>
===DONE===
--EXPECT--
string(9) "image/gif"
string(25) "image/gif; charset=binary"
===DONE===
