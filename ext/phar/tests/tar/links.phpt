--TEST--
Phar: tar with hard link and symbolic link
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
	$p = new PharData(dirname(__FILE__) . '/files/links.tar');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
var_dump($p['testit/link']->getContent());
var_dump($p['testit/hard']->getContent());
var_dump($p['testit/file']->getContent());
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
string(2) "hi"
string(2) "hi"
string(2) "hi"
===DONE===
