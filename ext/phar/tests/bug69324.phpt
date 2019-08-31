--TEST--
Bug #69324: Buffer Over-read in unserialize when parsing Phar
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--FILE--
<?php
try {
$p = new Phar(__DIR__.'/bug69324.phar', 0);
$meta=$p->getMetadata();
var_dump($meta);
} catch(Exception $e) {
	echo $e->getMessage();
}
--EXPECTF--
internal corruption of phar "%s" (truncated manifest entry)
