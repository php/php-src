--TEST--
Bug #68398: msooxml matches too many archives
--EXTENSIONS--
fileinfo
--FILE--
<?php

$f = new finfo(FILEINFO_MIME);
var_dump($f->file(__DIR__ . DIRECTORY_SEPARATOR . '68398.zip'));
?>
+++DONE+++
--EXPECT--
string(31) "application/zip; charset=binary"
+++DONE+++
