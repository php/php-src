--TEST--
Phar::compressFiles(Phar::GZ) zip format
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not present"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip.php';
$pname = 'phar://' . $fname;

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

$phar->compressFiles(Phar::GZ);
var_dump(file_get_contents($pname . '/a'));
var_dump($phar['a']->isCompressed(Phar::BZ2));
var_dump($phar['a']->isCompressed(Phar::GZ));
var_dump(file_get_contents($pname . '/b'));
var_dump($phar['b']->isCompressed(Phar::BZ2));
var_dump($phar['b']->isCompressed(Phar::GZ));
var_dump(file_get_contents($pname . '/c'));
var_dump($phar['b']->isCompressed(Phar::BZ2));
var_dump($phar['c']->isCompressed(Phar::GZ));

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip.php');
?>
--EXPECT--
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
===DONE===
