--TEST--
Phar: corrupted zip (no end of zip record)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
	new PharData(dirname(__FILE__) . '/files/nozipend.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: end of central directory not found in zip-based phar "%snozipend.zip"
===DONE===
