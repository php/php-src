--TEST--
Test FILEINFO_APPLE flag
--EXTENSIONS--
fileinfo
--FILE--
<?php

$f = new finfo;
var_dump($f->file(__DIR__ . "/resources/test.jpg", FILEINFO_APPLE));
var_dump($f->file(__DIR__ . "/resources/test.gif", FILEINFO_APPLE));
var_dump($f->file(__DIR__ . "/resources/test.ppt", FILEINFO_APPLE));

?>
--EXPECT--
string(8) "8BIMJPEG"
string(8) "8BIMGIFf"
string(8) "????PPT3"
