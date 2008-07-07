--TEST--
Phar: Phar::extractTo() - check that phar exists
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname   = dirname(__FILE__) . '/files/bogus.zip';
$fname2  = dirname(__FILE__) . '/files/notbogus.zip';
$extract = dirname(__FILE__) . '/test';

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
$dir = dirname(__FILE__) . '/test/';
@unlink($dir . 'stuff.txt');
@unlink($dir . 'nonsense.txt');
@rmdir($dir);
?>
--EXPECTF--
Invalid argument, %sfiles/bogus.zip cannot be found
phar://%sfiles/notbogus.zip%cnonsense.txt
phar://%sfiles/notbogus.zip%cstuff.txt
===DONE===
