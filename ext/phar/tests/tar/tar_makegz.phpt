--TEST--
Phar: tar-based phar, make new gzipped tar
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/tar_makegz.phar.tar';
$fname2 = __DIR__ . '/tar_makegz.phar.tar.gz';
$fname3 = __DIR__ . '/tar_makegz_b.phar.tar.gz';

$phar = new Phar($fname);
$phar['test'] = 'hi';
var_dump($phar->isFileFormat(Phar::TAR));
$phar = $phar->compress(Phar::GZ);


copy($fname2, $fname3);

$phar2 = new Phar($fname3);
var_dump($phar2->isFileFormat(Phar::TAR));
var_dump($phar2->isCompressed() == Phar::GZ);

?>
===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . '/tar_makegz.phar.gz');
@unlink(__DIR__ . '/tar_makegz.phar.tar');
@unlink(__DIR__ . '/tar_makegz.phar.tar.gz');
@unlink(__DIR__ . '/tar_makegz_b.phar.tar.gz');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
===DONE===
