--TEST--
Phar: Phar::extractTo() - check that phar exists
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname   = __DIR__ . '/files/bogus.zip';
$fname2  = __DIR__ . '/files/notbogus.zip';
$extract = __DIR__ . '/test-extract3';

$phar = new PharData($fname);

try {
	$phar->extractTo($extract);
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

$phar = new PharData($fname2);
foreach ($phar as $filename) {
	echo "$filename\n";
}

try {
	$phar->extractTo($extract);
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

?>
===DONE===
--CLEAN--
<?php
$dir = __DIR__ . '/test-extract3/';
@unlink($dir . 'stuff.txt');
@unlink($dir . 'nonsense.txt');
@rmdir($dir);
?>
--EXPECTF--
Invalid argument, %sfiles/bogus.zip cannot be found
phar://%sfiles/notbogus.zip%cnonsense.txt
phar://%sfiles/notbogus.zip%cstuff.txt
===DONE===
