--TEST--
Phar: PharFileInfo::__destruct
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

$a = new Phar($fname);
$a['a/subdir/here'] = 'hi';

$b = new PharFileInfo($pname . '/a/subdir');
unset($b);

$b = new PharFileInfo($pname . '/a/subdir/here');
unset($b);
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECTF--
===DONE===