--TEST--
Phar: copy()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '2.phar.php';

$pname = 'phar://'.$fname;
$iname = '/file.txt';
$ename = '/error/..';

$p = new Phar($fname);

try
{
	$p['a'] = 'hi';
	$p->startBuffering();
	$p->copy('a', 'b');
	echo file_get_contents($p['b']->getPathName());
	$p['a']->compress(Phar::GZ);
	$p['b']->setMetadata('a');
	$p->copy('b', 'c');
	$p->stopBuffering();
	echo file_get_contents($p['c']->getPathName());
	copy($fname, $fname2);
	$p->copy('a', $ename);
}
catch(Exception $e)
{
	echo $e->getMessage() . "\n";
}
ini_set('phar.readonly',1);
$p2 = new Phar($fname2);
echo "\n";
echo 'a: ' , file_get_contents($p2['a']->getPathName());
echo 'b: ' ,file_get_contents($p2['b']->getPathName());
echo 'c: ' ,file_get_contents($p2['c']->getPathName()), $p2['c']->getMetaData();
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar.php'); ?>
--EXPECTF--
hihifile "/error/.." contains invalid characters upper directory reference, cannot be copied from "a" in phar %s

a: hib: hic: hia===DONE===