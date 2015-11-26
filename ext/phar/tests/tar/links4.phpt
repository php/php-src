--TEST--
Phar: tar with link to root directory file from root directory file
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
try {
	$p = new PharData(dirname(__FILE__) . '/files/tinylink.tar');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
echo $p['file.txt']->getContent();
echo $p['link.txt']->getContent();
?>
===DONE===
--EXPECT--
hi
hi
===DONE===
