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

$fname = dirname(__FILE__) . '/tar_makegz.phar.tar';
$alias = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/tar_makegz_b.phar.tar';
$alias2 = 'phar://' . $fname2;

$phar = new Phar($fname);
$phar['test'] = 'hi';
var_dump($phar->isTar());
$phar->compressAllFilesGZ();

copy($fname, $fname2);

$phar2 = new Phar($fname2);
var_dump($phar2->isTar());
var_dump($phar2->isCompressed() == Phar::GZ);

?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_makegz.phar.tar');
@unlink(dirname(__FILE__) . '/tar_makegz_b.phar.tar');
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
===DONE===