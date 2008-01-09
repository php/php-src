--TEST--
Phar: mkdir test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$phar = new Phar($fname);

$phar['test/'] = '';
var_dump($phar['test']->isDir());
var_dump($phar['test/']->isDir());
mkdir($pname . '/another/dir/');
var_dump($phar['another/dir']->isDir());
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
bool(true)
bool(true)
bool(true)
