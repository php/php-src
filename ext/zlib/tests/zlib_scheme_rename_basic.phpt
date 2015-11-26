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
rename($srcFile, 'something.tmp');
var_dump(file_exists($inputFileName));
?>
===DONE===
--EXPECTF--

Warning: rename(): ZLIB wrapper does not support renaming in %s on line %d
bool(true)
===DONE===