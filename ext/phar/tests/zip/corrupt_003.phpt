--TEST--
Phar: corrupted zip (truncated file comment)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
	new PharData(__DIR__ . '/files/filecomment.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: corrupt zip archive, zip file comment truncated in zip-based phar "%sfilecomment.zip"
===DONE===
