--TEST--
finfo_open() works with Unicode magic file path (bug #71527)
--EXTENSIONS--
fileinfo
--INI--
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--FILE--
<?php
$magic = __DIR__ . DIRECTORY_SEPARATOR . "bug71527私はガラスを食べられます.magic";
$finfo = finfo_open(FILEINFO_NONE, $magic);
var_dump($finfo instanceof finfo);
?>
--EXPECT--
bool(true)
