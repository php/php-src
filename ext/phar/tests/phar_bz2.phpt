--TEST--
Phar: bzipped phar
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not available"); ?>
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
$b = new Phar($fname2);
var_dump($b->isPhar());
var_dump($b->isCompressed() == Phar::BZ2);
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
===DONE===