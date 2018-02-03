--TEST--
Phar object: add file
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$pharconfig = 0;

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setInfoClass('SplFileObject');

$phar['hi/f.php'] = 'hi';
var_dump(isset($phar['hi']));
var_dump(isset($phar['hi/f.php']));
echo $phar['hi/f.php'];
echo "\n";

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/files/phar_oo_011.phar.php');
__halt_compiler();
?>
--EXPECT--
bool(true)
bool(true)
hi
===DONE===
