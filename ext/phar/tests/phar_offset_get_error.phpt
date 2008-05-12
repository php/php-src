--TEST--
Phar: ignore filenames starting with / on offsetSet
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://'.$fname;
$iname = '/file.txt';
$ename = '/error/..';

$p = new Phar($fname);
$p[$iname] = "foobar\n";

try
{
	$p[$ename] = "foobar\n";
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
--EXPECT--
Entry /error/.. does not exist and cannot be created: phar error: invalid path "/error/.." contains upper directory reference
foobar
===DONE===
