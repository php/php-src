--TEST--
Bug #72660 (NULL Pointer dereference in zend_virtual_cwd)
--EXTENSIONS--
zip
--SKIPIF--
<?php
if(!PHP_ZTS) { die('skip ZTS required'); }
?>
--FILE--
<?php
$zip = new ZipArchive();
$zip->open(__DIR__ . "/bug72660.zip", ZIPARCHIVE::CREATE);
$zip->addPattern("/noexists/");
?>
okey
--CLEAN--
<?php
@unlink(__DIR__ . "/bug72660.zip");
?>
--EXPECT--
okey
