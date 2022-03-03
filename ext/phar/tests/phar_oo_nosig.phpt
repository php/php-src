--TEST--
Phar::getSignature() no signature
--EXTENSIONS--
phar
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
unlink(__DIR__ . '/files/phar_oo_nosig.phar.php');
?>
--EXPECT--
bool(false)
