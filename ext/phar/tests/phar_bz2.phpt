--TEST--
Phar: bzipped phar
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (!extension_loaded("bz2")) die("skip bz2 not available");
?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/phar_bz2.phar';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/phar_bz2.2.phar';
$pname2 = 'phar://' . $fname2;

$file = '<?php
Phar::mapPhar();
var_dump("it worked");
include "phar://" . __FILE__ . "/tar_004.php";
__HALT_COMPILER();';

$files = array();
$files['tar_004.php']   = '<?php var_dump(__FILE__);';
$files['internal/file/here']   = "hi there!\n";
$files['internal/dir/'] = '';
$files['dir/'] = '';
$bz2 = true;

include 'files/phar_test.inc';

include $fname;

$a = new Phar($fname);
$a['test'] = 'hi';
copy($fname, $fname2);
$a->setAlias('another');
$b = new Phar($fname2);
var_dump($b->isFileFormat(Phar::PHAR));
var_dump($b->isCompressed() == Phar::BZ2);
// additional code coverage
$b->isFileFormat(array());
try {
$b->isFileFormat(25);
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/phar_bz2.phar');
@unlink(dirname(__FILE__) . '/phar_bz2.2.phar');
?>
--EXPECTF--
string(9) "it worked"
string(%d) "phar://%sphar_bz2.phar/tar_004.php"
bool(true)
bool(true)

Warning: Phar::isFileFormat() expects parameter 1 to be int, array given in %sphar_bz2.php on line %d
Unknown file format specified
===DONE===
