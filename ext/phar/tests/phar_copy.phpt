--TEST--
Phar: copy()
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
<?php if (!extension_loaded("spl")) print "skip SPL not available"; ?>
<?php if (!extension_loaded("zlib")) print "skip zlib not available"; ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://'.$fname;
$iname = '/file.txt';
$ename = '/error/';

$p = new Phar($fname);
$p[$iname] = "foobar\n";

try
{
	$p['a'] = 'hi';
	$p->copy('a', 'b');
	echo $p['b'];
	$p['a']->setCompressedGZ();
	$p->copy('a', 'c');
	echo $p['c'];
	$p->copy('a', $ename);
}
catch(Exception $e)
{
	echo $e->getMessage() . "\n";
}

include($pname . $iname);
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
hihi
file "/error/" contains invalid characters /, cannot be copied from "a" in phar %s
===DONE===
