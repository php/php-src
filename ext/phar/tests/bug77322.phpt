--TEST--
Bug #77322 (PharData::addEmptyDir('/') Possible integer overflow)
--SKIPIF--
<?php
if (!extension_loaded('phar')) die('skip phar extension not available');
?>
--FILE--
<?php
$zip = new PharData(__DIR__ . '/bug77322.zip');
$zip->addEmptyDir('/');
var_dump($zip->count());

$tar = new PharData(__DIR__ . '/bug77322.tar');
$tar->addEmptyDir('/');
var_dump($tar->count());
?>
--EXPECT--
int(1)
int(1)
--CLEAN--
<?php
unlink(__DIR__ . '/bug77322.zip');
unlink(__DIR__ . '/bug77322.tar');
?>
