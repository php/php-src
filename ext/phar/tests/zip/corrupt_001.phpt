--TEST--
Phar: corrupted zip (count mismatch)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
try {
	new PharData(dirname(__FILE__) . '/files/count1.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	new PharData(dirname(__FILE__) . '/files/count2.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: corrupt zip archive, conflicting file count in end of central directory record in zip-based phar "%scount1.zip"
phar error: corrupt zip archive, conflicting file count in end of central directory record in zip-based phar "%scount2.zip"
===DONE===
