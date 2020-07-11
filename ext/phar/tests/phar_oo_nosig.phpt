--TEST--
Phar::getSignature() no signature
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
var_dump($phar->getSignature());
?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECT--
bool(false)
