--TEST--
Phar: require hash
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=1
--FILE--
<?php

$pharconfig = 0;

require_once 'files/phar_oo_test.inc';

try {
Phar::loadPhar($fname);
} catch (Exception $e) {
echo $e->getMessage();
}

?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/files/032.phar.php');
__halt_compiler();
?>
--EXPECTF--
phar "%s032.phar.php" does not have a signature===DONE===
