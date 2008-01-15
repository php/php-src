--TEST--
Phar: tar-based phar, make new bzipped tar
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/tar_makebz2.phar.tar';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/tar_makebz22.phar.tar';
$pname2 = 'phar://' . $fname2;

$a = new Phar($fname);
$a['test'] = 'hi';
var_dump($a->isTar());
$a->compressAllFilesBZIP2();
copy($fname, $fname2);
$b = new Phar($fname2);
var_dump($b->isTar());
var_dump($b->isCompressed() == Phar::BZ2);
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_makebz2.phar.tar');
@unlink(dirname(__FILE__) . '/tar_makebz22.phar.tar');
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
===DONE===