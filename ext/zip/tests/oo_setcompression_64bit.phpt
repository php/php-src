--TEST--
setCompressionName and setCompressionIndex methods
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

$zip = new ZipArchive;

try {
	$zip->setCompressionName('entry5.txt', PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	$zip->setCompressionIndex(4, PHP_INT_MAX);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
ZipArchive::setCompressionName(): Argument #2 ($method) must be between -1 and %d
ZipArchive::setCompressionIndex(): Argument #2 ($method) must be between -1 and %d
