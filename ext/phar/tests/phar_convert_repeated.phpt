--TEST--
Phar::convertToZip|Tar|Phar() repeated (phar_based archives)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$fname3 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.tar';
$stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';
$file = $stub;

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

echo "=================== new Phar() =======================\n";
$phar = new Phar($fname);
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToTar() =====================\n";

$phar->convertToTar();
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

$phar->convertToPhar();
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump(strlen($phar->getStub()));
var_dump($phar->getAlias());

echo "================= convertToZip() =====================\n";

$phar->convertToZip();
var_dump($phar->isPhar());
var_dump($phar->isTar());
var_dump($phar->isZip());
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToTar() =====================\n";

$phar->convertToTar();
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

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
?>
--EXPECT--
=================== new Phar() =======================
bool(true)
bool(false)
bool(false)
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
string(3) "hio"
================= convertToTar() =====================
bool(false)
bool(true)
bool(false)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
string(57) "C:/sandbox/pecl/phar/tests/phar_convert_repeated.phar.tar"
================= convertToZip() =====================
bool(false)
bool(false)
bool(true)
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
string(57) "C:/sandbox/pecl/phar/tests/phar_convert_repeated.phar.zip"
================= convertToPhar() ====================
bool(true)
bool(false)
bool(false)
int(6573)
string(53) "C:/sandbox/pecl/phar/tests/phar_convert_repeated.phar"
================= convertToZip() =====================
bool(false)
bool(false)
bool(true)
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
string(57) "C:/sandbox/pecl/phar/tests/phar_convert_repeated.phar.zip"
================= convertToTar() =====================
bool(false)
bool(true)
bool(false)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
string(57) "C:/sandbox/pecl/phar/tests/phar_convert_repeated.phar.tar"
================= convertToZip() =====================
bool(false)
bool(false)
bool(true)
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
string(57) "C:/sandbox/pecl/phar/tests/phar_convert_repeated.phar.zip"
===DONE===
