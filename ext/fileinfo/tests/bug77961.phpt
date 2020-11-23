--TEST--
Bug #77961 (finfo_open crafted magic parsing SIGABRT)
--SKIPIF--
<?php
if (!extension_loaded('fileinfo')) die('skip fileinfo extension not available');
?>
--FILE--
<?php
finfo_open(FILEINFO_NONE, __DIR__ . '/bug77961.magic');
?>
--EXPECTF--
Notice: finfo_open(): Warning: Expected numeric type got `indirect' in %s on line %d

Fatal error: fatal libmagic error in %s on line %d
