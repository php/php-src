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
$tar = $phar->convertToExecutable(Phar::TAR);
echo $tar->getPath() . "\n";
$tgz = $tar->convertToExecutable(null, Phar::GZ);
echo $tgz->getPath() . "\n";
try {
$tgz->convertToExecutable(25);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.zip');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar.gz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar.bz2');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.tbz');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '2.phar.zip');
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
%sphar_convert_again2.phar.tar
%sphar_convert_again2.phar.tar.gz
Unknown file format specified, please pass one of Phar::PHAR, Phar::TAR or Phar::ZIP
===DONE===
