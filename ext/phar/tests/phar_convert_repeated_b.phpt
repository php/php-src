--TEST--
PharData::convertToZip|Tar|Phar() repeated (phardata_based archives)
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.zip';

echo "=================== new PharData() ==================\n";
$phar = new PharData($fname);
$phar['a'] = 'a';
$phar['b'] = 'b';
$phar['c'] = 'c';

var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToTar() =====================\n";

$phar = $phar->convertToData(Phar::TAR);
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToZip() =====================\n";

$phar = $phar->convertToData(Phar::ZIP, Phar::NONE, '.1.zip');
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToPhar() ====================\n";

try {
    $phar = $phar->convertToExecutable(Phar::PHAR);
    var_dump($phar->isFileFormat(Phar::PHAR));
    var_dump($phar->isFileFormat(Phar::TAR));
    var_dump($phar->isFileFormat(Phar::ZIP));
    var_dump(strlen($phar->getStub()));
    var_dump($phar->getAlias());
} catch(Exception $e) {
    echo $e->getMessage()."\n";
}

echo "================ convertToTar(GZ) ====================\n";

$phar = $phar->convertToData(Phar::TAR, Phar::GZ, '.2.tar');
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToPhar() ====================\n";

try {
    $phar = $phar->convertToExecutable(Phar::PHAR);
    var_dump($phar->isFileFormat(Phar::PHAR));
    var_dump($phar->isFileFormat(Phar::TAR));
    var_dump($phar->isFileFormat(Phar::ZIP));
    var_dump(strlen($phar->getStub()));
    var_dump($phar->getAlias());
} catch(Exception $e) {
    echo $e->getMessage()."\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.1.2.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.zip');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.1.zip');
?>
--EXPECT--
=================== new PharData() ==================
bool(false)
bool(false)
bool(true)
string(0) ""
NULL
================= convertToTar() =====================
bool(false)
bool(true)
bool(false)
string(0) ""
NULL
================= convertToZip() =====================
bool(false)
bool(false)
bool(true)
string(0) ""
NULL
================= convertToPhar() ====================
Cannot write out executable phar archive, phar is read-only
================ convertToTar(GZ) ====================
bool(false)
bool(true)
bool(false)
string(0) ""
NULL
================= convertToPhar() ====================
Cannot write out executable phar archive, phar is read-only
