--TEST--
Phar: unable to process zip (zip spanning multiple archives)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
try {
	new PharData(dirname(__FILE__) . '/files/disknumber.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar error: split archives spanning multiple zips cannot be processed in zip-based phar "%sdisknumber.zip"
===DONE===
