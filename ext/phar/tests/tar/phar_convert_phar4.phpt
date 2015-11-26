--TEST--
Phar::convertToPhar() with global metadata
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '2.phar';

$phar = new Phar($fname);
$phar['a.txt'] = 'some text';
$phar->setMetadata(b'hi');
$phar->stopBuffering();
var_dump($phar->isFileFormat(Phar::TAR));
var_dump(strlen($phar->getStub()));
var_dump($phar->getMetadata());

$phar = $phar->convertToExecutable(Phar::TAR);
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->getStub());
var_dump($phar->getMetadata());

$phar['a'] = 'hi there';

$phar = $phar->convertToExecutable(Phar::PHAR, Phar::GZ);
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isCompressed());
var_dump(strlen($phar->getStub()));
var_dump($phar->getMetadata());

copy($fname . '.gz', $fname2);

$phar = new Phar($fname2);
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isCompressed() == Phar::GZ);
var_dump(strlen($phar->getStub()));
var_dump($phar->getMetadata());

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.gz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.gz');
__HALT_COMPILER();
?>
--EXPECT--
bool(false)
int(6683)
string(2) "hi"
bool(true)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
string(2) "hi"
bool(true)
int(4096)
int(6683)
string(2) "hi"
bool(true)
bool(true)
int(6683)
string(2) "hi"
===DONE===
