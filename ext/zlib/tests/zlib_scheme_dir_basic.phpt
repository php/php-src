--TEST--
Test compress.zlib:// scheme with the directory functions
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$inputFileName = __DIR__."/dir.gz";
$srcFile = "compress.zlib://$inputFileName";
var_dump(mkdir($srcFile));
var_dump(is_dir($srcFile));
var_dump(opendir($srcFile));
var_dump(rmdir($srcFile));
?>
===DONE===
--EXPECTF--
bool(false)
bool(false)

Warning: opendir(): failed to open dir (compress.zlib://%s/dir.gz): not implemented in %s on line %d
bool(false)
bool(false)
===DONE===
