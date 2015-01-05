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
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToTar() =====================\n";

$phar = $phar->convertToExecutable(Phar::TAR);
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToZip() =====================\n";

$phar = $phar->convertToExecutable(Phar::ZIP);
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToPhar() ====================\n";

$phar = $phar->convertToExecutable(Phar::PHAR, Phar::NONE, '.2.phar');
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump(strlen($phar->getStub()));
var_dump($phar->getAlias());

echo "================= convertToZip() =====================\n";

$phar = $phar->convertToExecutable(Phar::ZIP, Phar::NONE, '.2.phar.zip');
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToTar() =====================\n";

$phar = $phar->convertToExecutable(Phar::TAR, Phar::NONE, '2.phar.tar');
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

echo "================= convertToZip() =====================\n";

$phar = $phar->convertToExecutable(Phar::ZIP, Phar::NONE, '3.phar.zip');
var_dump($phar->isFileFormat(Phar::PHAR));
var_dump($phar->isFileFormat(Phar::TAR));
var_dump($phar->isFileFormat(Phar::ZIP));
var_dump($phar->getStub());
var_dump($phar->getAlias());

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.3.phar.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.3.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.3.phar');
?>
--EXPECTF--
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
string(%d) "%sphar_convert_repeated.phar.tar"
================= convertToZip() =====================
bool(false)
bool(false)
bool(true)
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
NULL
================= convertToPhar() ====================
bool(true)
bool(false)
bool(false)
int(6683)
NULL
================= convertToZip() =====================
bool(false)
bool(false)
bool(true)
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
NULL
================= convertToTar() =====================
bool(false)
bool(true)
bool(false)
string(60) "<?php // tar-based phar archive stub file
__HALT_COMPILER();"
NULL
================= convertToZip() =====================
bool(false)
bool(false)
bool(true)
string(60) "<?php // zip-based phar archive stub file
__HALT_COMPILER();"
NULL
===DONE===
