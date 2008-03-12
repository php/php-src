--TEST--
PharData::convertToZip|Tar|Phar() repeated (phardata_based archives)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
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

$phar->convertToTar(Phar::GZ);
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToZip() =====================\n";

$phar->convertToZip();
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToPhar() ====================\n";

try {
	$phar->convertToPhar();
	var_dump($phar->isPhar());
	var_dump($phar->isTar());
	var_dump($phar->isZip());
	var_dump(strlen($phar->getStub()));
	var_dump($phar->getAlias());
} catch(Exception $e) {
	echo $e->getMessage()."\n";
}

echo "================ convertToTar(GZ) ====================\n";

$phar->convertToTar(Phar::GZ);
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToPhar() ====================\n";

try {
	$phar->convertToPhar();
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
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
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
bool(true)
bool(false)
bool(false)
int(6651)
NULL
================ convertToTar(GZ) ====================
bool(false)
bool(true)
bool(false)
string(0) ""
NULL
================= convertToPhar() ====================
bool(true)
bool(false)
bool(false)
int(6651)
NULL
===DONE===
