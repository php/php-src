--TEST--
Test FILEINFO_EXTENSION flag
--EXTENSIONS--
fileinfo
--FILE--
<?php

$f = new finfo;
var_dump($f->file(__DIR__ . "/resources/test.jpg", FILEINFO_EXTENSION));

?>
--EXPECT--
string(17) "jpeg/jpg/jpe/jfif"
