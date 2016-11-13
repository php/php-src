--TEST--
Phar: tar with hard link and symbolic link
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';
copy(__DIR__ . '/files/links.tar', $fname);
try {
	$p = new PharData($fname);
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
var_dump($p['testit/link']->getContent());
var_dump($p['testit/hard']->getContent());
var_dump($p['testit/file']->getContent());
$p['testit/link'] = 'overwriting';
var_dump($p['testit/link']->getContent());
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECT--
string(3) "hi
"
string(3) "hi
"
string(3) "hi
"
string(11) "overwriting"
===DONE===
