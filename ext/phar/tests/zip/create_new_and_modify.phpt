--TEST--
Phar: create and modify zip-based phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip.php';
$pname = 'phar://' . $fname;

@unlink($fname);

file_put_contents($pname . '/a.php', "brand new!\n");

$phar = new Phar($fname);
var_dump($phar->isFileFormat(Phar::ZIP));
$sig1 = md5_file($fname);

include $pname . '/a.php';

file_put_contents($pname .'/a.php', "modified!\n");
file_put_contents($pname .'/b.php', "another!\n");

$phar = new Phar($fname);
$sig2 = md5_file($fname);

var_dump($sig1 != $sig2);

include $pname . '/a.php';
include $pname . '/b.php';

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip.php'); ?>
--EXPECTF--
bool(true)
brand new!
bool(true)
modified!
another!
===DONE===
