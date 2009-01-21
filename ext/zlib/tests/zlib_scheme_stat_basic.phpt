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
$inputFileName = dirname(__FILE__)."/004.txt.gz";
$srcFile = "compress.zlib://$inputFileName";
stat($srcFile);
lstat($srcFile);
?>
===DONE===
--EXPECTF--

Warning: stat(): stat failed for compress.zlib://%s/004.txt.gz in %s on line %d

Warning: lstat(): Lstat failed for compress.zlib://%s/004.txt.gz in %s on line %d
===DONE===