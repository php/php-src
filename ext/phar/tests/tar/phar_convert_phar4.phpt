--TEST--
Phar::convertToPhar() with global metadata
--EXTENSIONS--
phar
zlib
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '2.phar';

$phar = new Phar($fname);
$phar['a.txt'] = 'some text';
$phar->setMetadata('hi');
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
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.gz');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '2.phar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
?>
--EXPECT--
bool(false)
int(6641)
string(2) "hi"
bool(true)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
string(2) "hi"
bool(true)
int(4096)
int(6641)
string(2) "hi"
bool(true)
bool(true)
int(6641)
string(2) "hi"
