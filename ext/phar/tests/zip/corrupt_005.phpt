--TEST--
Phar: encrypted zip
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
try {
	new PharData(__DIR__ . '/files/encrypted.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: Cannot process encrypted zip files in zip-based phar "%sencrypted.zip"
===DONE===
