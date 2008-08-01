--TEST--
Phar: a non-executable zip with no stub named .phar.zip
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=1
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$pname = 'phar://' . $fname;

copy(dirname(__FILE__) . '/files/zip.zip', $fname);
include $fname;
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
__HALT_COMPILER();
?>
--EXPECTF--
Warning: include(phar://%snotphar.phar.zip/.phar/stub.php): failed to open stream: '%snotphar.phar.zip' is not a phar archive. Use PharData::__construct() for a standard zip or tar archive in %snotphar.php on line %d
PK%a
