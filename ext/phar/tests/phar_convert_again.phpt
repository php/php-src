--TEST--
Phar::conversion to other formats
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip no zlib"); ?>
<?php if (!extension_loaded("bz2")) die("skip no bz2"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '2.tbz';
$pname = 'phar://' . $fname;
$stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';
$file = $stub;

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);
$zip = $phar->convertToData(Phar::ZIP);
echo $zip->getPath() . "\n";
$tgz = $phar->convertToData(Phar::TAR, Phar::GZ);
echo $tgz->getPath() . "\n";
$tbz = $phar->convertToData(Phar::TAR, Phar::BZ2);
echo $tbz->getPath() . "\n";
try {
$phar = $tbz->convertToExecutable(Phar::PHAR, Phar::NONE);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
copy($tbz->getPath(), $fname2);
$tbz = new PharData($fname2);
$phar = $tbz->convertToExecutable(Phar::PHAR, Phar::NONE);
echo $phar->getPath() . "\n";
$phar['a'] = 'hi';
$phar['a']->setMetadata('hi');
$zip = $phar->convertToExecutable(Phar::ZIP);
echo $zip->getPath() . "\n";
echo $zip['a']->getMetadata() . "\n";
$data = $zip->convertToData();
echo $data->getPath() . "\n";
// extra code coverage
try {
$data->setStub('hi');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$data->setDefaultStub();
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$data->setAlias('hi');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$tar = $phar->convertToExecutable(Phar::TAR);
echo $tar->getPath() . "\n";
$data = $tar->convertToData();
echo $data->getPath() . "\n";
$tgz = $tar->convertToExecutable(null, Phar::GZ);
echo $tgz->getPath() . "\n";
try {
$tgz->convertToExecutable(25);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToExecutable(Phar::ZIP, Phar::GZ);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToExecutable(Phar::ZIP, Phar::BZ2);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$phar->convertToData();
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToData(Phar::PHAR);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToData(25);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToData(Phar::ZIP, Phar::GZ);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToData(Phar::ZIP, Phar::BZ2);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToExecutable(Phar::TAR, 25);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToData(Phar::TAR, 25);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
// extra code coverage
try {
$data->setStub('hi');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$data->setAlias('hi');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$data->setDefaultStub();
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$tgz->convertToData(Phar::TAR, Phar::GZ, '.phar.tgz.oops');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}

try {
$phar->convertToExecutable(Phar::TAR, Phar::GZ, '.tgz.oops');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}

try {
$tgz->convertToData(Phar::TAR, Phar::GZ, '.phar/.tgz.oops');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.gz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar.gz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar.bz2');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.tbz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar.tar.gz');
__HALT_COMPILER();
?>
--EXPECTF--
%sphar_convert_again.zip
%sphar_convert_again.tar.gz
%sphar_convert_again.tar.bz2
Unable to add newly converted phar "%sphar_convert_again.phar" to the list of phars, a phar with that name already exists
%sphar_convert_again2.phar
%sphar_convert_again2.phar.zip
hi
%sphar_convert_again2.zip
A Phar stub cannot be set in a plain zip archive
A Phar stub cannot be set in a plain zip archive
A Phar alias cannot be set in a plain zip archive
%sphar_convert_again2.phar.tar
%sphar_convert_again2.tar
%sphar_convert_again2.phar.tar.gz
Unknown file format specified, please pass one of Phar::PHAR, Phar::TAR or Phar::ZIP
Cannot compress entire archive with gzip, zip archives do not support whole-archive compression
Cannot compress entire archive with bz2, zip archives do not support whole-archive compression
Cannot write out data phar archive, use Phar::TAR or Phar::ZIP
Cannot write out data phar archive, use Phar::TAR or Phar::ZIP
Unknown file format specified, please pass one of Phar::TAR or Phar::ZIP
Cannot compress entire archive with gzip, zip archives do not support whole-archive compression
Cannot compress entire archive with bz2, zip archives do not support whole-archive compression
Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2
Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2
A Phar stub cannot be set in a plain tar archive
A Phar alias cannot be set in a plain tar archive
A Phar stub cannot be set in a plain tar archive
data phar "%sphar_convert_again2.phar.tgz.oops" has invalid extension phar.tgz.oops
phar "%sphar_convert_again2.tgz.oops" has invalid extension tgz.oops
data phar "%sphar_convert_again2.phar/.tgz.oops" has invalid extension phar/.tgz.oops
===DONE===
