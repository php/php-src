--TEST--
Phar: PHP bug #48377 "error message unclear on converting phar with existing file" test #2
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.zip';

$phar = new PharData($fname);
$phar['x'] = 'hi';
try {
	$phar->convertToData(Phar::ZIP, Phar::NONE, 'phar.zip');
} catch (BadMethodCallException $e) {
	echo $e->getMessage(),"\n";
}
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.zip');?>
--EXPECTF--
data phar "%sbug48377.2.phar.zip" has invalid extension phar.zip
===DONE===
