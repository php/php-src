--TEST--
Test compress.zlib:// scheme with the fopen on a file scheme
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?>
--FILE--
<?php
$inputFileName = dirname(__FILE__)."/004.txt.gz";
$srcFile = "file://$inputFileName";
$compressedFile = "compress.zlib://$srcFile";

echo "file=$compressedFile\n\n";
$h = fopen($compressedFile, 'r');
fpassthru($h);
fclose($h);
?>
===DONE===
--EXPECTF--
file=compress.zlib://file://%s/004.txt.gz

When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
===DONE===