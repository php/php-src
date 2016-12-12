--TEST--
Bug #72660 (NULL Pointer dereference in zend_virtual_cwd)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
if(PHP_ZTS == 0) { die('skip ZTS required'); }
?>
--FILE--
<?php
$zip = new ZipArchive();
$zip->open(dirname(__FILE__) . "/bug72660.zip", ZIPARCHIVE::CREATE);
$zip->addPattern("/noexists/");
?>
okey
--CLEAN--
<?php
@unlink(dirname(__FILE__) . "/bug72660.zip");
?>
--EXPECT--
okey
