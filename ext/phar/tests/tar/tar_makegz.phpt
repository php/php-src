--TEST--
Phar: tar-based phar, make new gzipped tar
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/tar_makegz.phar.tar';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/tar_makegz2.phar.tar';
$pname2 = 'phar://' . $fname2;

$a = new Phar($fname);
$a['test'] = 'hi';
var_dump($a->isTar());
$a->compressAllFilesGZ();
copy($fname, $fname2);
$b = new Phar($fname2);
var_dump($b->isTar());
var_dump($b->isCompressed() == Phar::GZ);
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_makegz.phar.tar');
@unlink(dirname(__FILE__) . '/tar_makegz2.phar.tar');
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
===DONE===