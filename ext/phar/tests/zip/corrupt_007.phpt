--TEST--
Phar: corrupted zip (truncated filename record)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
try {
	new PharData(dirname(__FILE__) . '/files/truncfilename.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: corrupted central directory entry, no magic signature in zip-based phar "%struncfilename.zip"
===DONE===
