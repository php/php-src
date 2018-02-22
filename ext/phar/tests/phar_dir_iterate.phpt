--TEST--
Phar object: iterate test with sub-directories and RecursiveIteratorIterator
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';

$phar = new Phar($fname);
$phar['top.txt'] = 'hi';
$phar['sub/top.txt'] = 'there';
$phar['another.file.txt'] = 'wowee';
$newphar = new Phar($fname);
foreach (new RecursiveIteratorIterator($newphar) as $path => $obj) {
	var_dump($obj->getPathName());
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
__halt_compiler();
?>
--EXPECTF--
string(%d) "phar://%sphar_dir_iterate.phar.php%canother.file.txt"
string(%d) "phar://%sphar_dir_iterate.phar.php/sub%ctop.txt"
string(%d) "phar://%sphar_dir_iterate.phar.php%ctop.txt"
===DONE===
