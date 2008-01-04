--TEST--
Phar front controller php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

$a = new Phar($fname);
$a['a.php'] = 'hio';
$a->setStub('<?php
Phar::webPhar();
__HALT_COMPILER();');

$_SERVER['REQUEST_URI'] = '/' . basename(__FILE__, '.php') . '.phar.php/a.php';
include $fname;
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phartmp.php');
__HALT_COMPILER();
?>
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
hio
