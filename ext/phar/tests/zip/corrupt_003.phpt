--TEST--
Phar: corrupted zip (truncated file comment)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
try {
	new PharData(dirname(__FILE__) . '/files/filecomment.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: corrupt zip archive, zip file comment truncated in zip-based phar "%sfilecomment.zip"
===DONE===
