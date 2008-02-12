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

$fname = dirname(__FILE__) . '/tar_makebz2.phar.tar';
$alias = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/tar_makebz2_b.phar.tar';
$alias2 = 'phar://' . $fname2;

$phar = new Phar($fname);
$phar['test'] = 'hi';
var_dump($phar->isTar());
$phar->compressAllFilesBZIP2();

copy($fname, $fname2);

$phar2 = new Phar($fname2);
var_dump($phar2->isTar());
var_dump($phar2->isCompressed() == Phar::BZ2);

?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_makebz2.phar.tar');
@unlink(dirname(__FILE__) . '/tar_makebz2_b.phar.tar');
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
===DONE===