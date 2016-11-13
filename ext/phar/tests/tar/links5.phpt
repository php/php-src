--TEST--
Phar: tar with relative link to subdirectory file from subdirectory file
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
try {
	$p = new PharData(__DIR__ . '/files/subdirlink.tar');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
echo $p['hi/test.txt']->getContent();
echo $p['hi/link.txt']->getContent();
?>
===DONE===
--EXPECT--
hi
hi
===DONE===
