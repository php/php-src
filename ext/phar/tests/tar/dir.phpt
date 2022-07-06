--TEST--
Phar: mkdir/rmdir test tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar.php';
$pname = 'phar://' . $fname;
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.1.phar.php';
$pname2 = 'phar://' . $fname2;
$fname3 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.php';
$pname3 = 'phar://' . $fname3;
$phar = new Phar($fname);
var_dump($phar->isFileFormat(Phar::TAR));

$phar->addEmptyDir('test');
var_dump(isset($phar['.phar/stub.php']));
var_dump($phar['test']->isDir());
var_dump($phar['test/']->isDir());
copy($fname, $fname2);
mkdir($pname . '/another/dir/');
var_dump($phar['another/dir']->isDir());
rmdir($pname . '/another/dir/');
copy($fname, $fname3);
clearstatcache();
var_dump(file_exists($pname . '/another/dir/'));
var_dump(file_exists($pname2 . '/test/'));
var_dump(file_exists($pname3 . '/another/dir/'));
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.php'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.1.phar.php'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php'); ?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
