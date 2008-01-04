--TEST--
Phar object: iterate test with sub-directories and RecursiveIteratorIterator
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$phar = new Phar(dirname(__FILE__) . '/test.phar');
$phar['top.txt'] = 'hi';
$phar['sub/top.txt'] = 'there';
$phar['another.file.txt'] = 'wowee';
$newphar = new Phar(dirname(__FILE__) . '/test.phar');
foreach (new RecursiveIteratorIterator($newphar) as $path => $obj) {
	var_dump($obj->getPathName());
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/test.phar');
__halt_compiler();
?>
--EXPECTF--
string(%d) "phar://%stest.phar%canother.file.txt"
string(%d) "phar://%stest.phar%csub%ctop.txt"
string(%d) "phar://%stest.phar%ctop.txt"
===DONE===
