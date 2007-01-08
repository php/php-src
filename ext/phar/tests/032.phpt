--TEST--
Phar: require hash
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=1
--FILE--
<?php

$pharconfig = 0;

require_once 'phar_oo_test.inc';

Phar::loadPhar($fname);

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECTF--

Catchable fatal error: Phar::loadPhar(): phar "%sphar_oo_test.phar.php" does not have a signature in %s032.php on line %d
