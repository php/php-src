--TEST--
Test compress.zlib:// scheme with the unlink function
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$inputFileName = __DIR__."/004.txt.gz";
$srcFile = "compress.zlib://$inputFileName";
unlink($srcFile);
var_dump(file_exists($inputFileName));
?>
===DONE===
--EXPECTF--
Warning: unlink(): ZLIB does not allow unlinking in %s on line %d
bool(true)
===DONE===
