--TEST--
Phar::compressFiles(Phar::BZ2) zip format
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not present"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip.php';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$pname = 'phar://' . $fname;
$pname2 = 'phar://' . $fname2;

$phar = new Phar($fname);
$phar['a'] = 'a';
$phar['b'] = 'b';
$phar['c'] = 'c';

var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressed());
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['b']->isCompressed());
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['c']->isCompressed());

$phar->compressFiles(Phar::BZ2);
var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressed(Phar::GZ));
var_dump($phar['a']->isCompressed(Phar::BZ2));
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['b']->isCompressed(Phar::GZ));
var_dump($phar['b']->isCompressed(Phar::BZ2));
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['c']->isCompressed(Phar::GZ));
var_dump($phar['b']->isCompressed(Phar::BZ2));
$phar['d'] = 'hi'; // increases code coverage by having ufp open
copy($fname, $fname2);
$c = new Phar($fname2);
var_dump(file_get_contents($pname2 . '/a'));
var_dump($c['a']->isCompressed(Phar::GZ));
var_dump($c['a']->isCompressed(Phar::BZ2));

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip.php');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
?>
--EXPECTF--
string(1) "a"
bool(false)
string(1) "b"
bool(false)
string(1) "c"
bool(false)
string(1) "a"
bool(false)
bool(true)
string(1) "b"
bool(false)
bool(true)
string(1) "c"
bool(false)
bool(true)
string(1) "a"
bool(false)
bool(true)
===DONE===
