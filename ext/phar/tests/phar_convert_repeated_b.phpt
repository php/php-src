--TEST--
PharData::convertToZip|Tar|Phar() repeated (phardata_based archives)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
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

var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToTar() =====================\n";

$phar = $phar->convertToTar();
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToZip() =====================\n";

$phar = $phar->convertToZip('.1.zip');
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToPhar() ====================\n";

try {
	$phar = $phar->convertToPhar();
	var_dump($phar->isPhar());
	var_dump($phar->isTar());
	var_dump($phar->isZip());
	var_dump(strlen($phar->getStub()));
	var_dump($phar->getAlias());
} catch(Exception $e) {
	echo $e->getMessage()."\n";
}

echo "================ convertToTar(GZ) ====================\n";

$phar = $phar->convertToTar('.2.tar')->compress(Phar::GZ);
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToPhar() ====================\n";

try {
	$phar = $phar->convertToPhar();
	var_dump($phar->isPhar());
	var_dump($phar->isTar());
	var_dump($phar->isZip());
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
Cannot write out phar archive, phar is read-only
================ convertToTar(GZ) ====================
bool(false)
bool(true)
bool(false)
string(0) ""
NULL
================= convertToPhar() ====================
Cannot write out phar archive, phar is read-only
===DONE===