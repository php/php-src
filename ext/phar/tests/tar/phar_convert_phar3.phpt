--TEST--
Phar::convertToPhar() bzipped
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("bz2")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '2.phar';

$phar = new Phar($fname);
$phar['a.txt'] = 'some text';
$phar->stopBuffering();
var_dump($phar->isFileFormat(Phar::TAR));
var_dump(strlen($phar->getStub()));

$phar = $phar->convertToExecutable(Phar::TAR);
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->getStub());

$phar['a'] = 'hi there';

$phar = $phar->convertToExecutable(Phar::PHAR, Phar::BZ2);
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isCompressed());
var_dump(strlen($phar->getStub()));

copy($fname . '.bz2', $fname2);

$phar = new Phar($fname2);
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isCompressed() == Phar::BZ2);
var_dump(strlen($phar->getStub()));

?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.bz2');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.phar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.bz2');
__HALT_COMPILER();
?>
--EXPECT--
bool(false)
int(6641)
bool(true)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
bool(true)
int(8192)
int(6641)
bool(true)
bool(true)
int(6641)
===DONE===
