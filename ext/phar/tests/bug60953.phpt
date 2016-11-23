--TEST--
Phar: rename test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$phar = new Phar(__DIR__ . '/package-1.2.3.phar');
$phar['hello.txt'] = 'Hello World';
$phar->stopBuffering();
unset($phar);
$phar = new Phar(__DIR__ . '/package-1.2.3.phar');
$phar->convertToExecutable(Phar::TAR, Phar::GZ);
var_dump(file_exists(__DIR__ . '/package-1.2.3.phar'));
var_dump(file_exists(__DIR__ . '/package-1.2.3.phar.tar.gz'));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/package-1.2.3.phar');
unlink(__DIR__ . '/package-1.2.3.phar.tar.gz');
?>
--EXPECTF--
bool(true)
bool(true)