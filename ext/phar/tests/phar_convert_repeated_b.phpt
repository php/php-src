--TEST--
PharData::convertToZip|Tar|Phar() repeated (phardata_based archives)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--INI--
phar.readonly=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.zip';

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
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.gz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar.gz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.1.zip');
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
===DONE===