--TEST--
Phar: a non-executable zip with no stub named .phar.zip
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=1
detect_unicode=0
zend.multibyte=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';
$pname = 'phar://' . $fname;

copy(__DIR__ . '/files/zip.zip', $fname);
include $fname;
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
__HALT_COMPILER();
?>
--EXPECTF--
Warning: include(phar://%snotphar.phar.zip/.phar/stub.php): Failed to open stream: '%snotphar.phar.zip' is not a phar archive. Use PharData::__construct() for a standard zip or tar archive in %snotphar.php on line %d
PK%a
