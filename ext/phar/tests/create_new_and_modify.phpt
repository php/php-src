--TEST--
Phar: create and modify phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

@unlink($fname);

file_put_contents($pname . '/a.php', "brand new!\n");

$phar = new Phar($fname);
$sig1 = $phar->getSignature();

include $pname . '/a.php';

file_put_contents($pname .'/a.php', "modified!\n");
file_put_contents($pname .'/b.php', "another!\n");

$phar = new Phar($fname);
$sig2 = $phar->getSignature();

var_dump($sig1[b'hash']);
var_dump($sig2[b'hash']);
var_dump($sig1[b'hash'] != $sig2[b'hash']);

include $pname . '/a.php';
include $pname . '/b.php';

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
brand new!
string(40) "%s"
string(40) "%s"
bool(true)
modified!
another!
===DONE===
