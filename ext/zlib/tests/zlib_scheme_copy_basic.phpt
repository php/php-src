--TEST--
Test compress.zlib:// scheme with the copy function: compressed to compressed
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
$inputFileName = __DIR__."/004.txt.gz";
$outputFileName = __FILE__.'.tmp';

$srcFile = "compress.zlib://$inputFileName";
$destFile = "compress.zlib://$outputFileName";
copy($srcFile, $destFile);

$h = gzopen($inputFileName, 'r');
$org_data = gzread($h, 4096);
gzclose($h);

$h = gzopen($outputFileName, 'r');
$copied_data = gzread($h, 4096);
gzclose($h);

if ($org_data == $copied_data) {
   echo "OK: Copy identical\n";
}
else {
   echo "FAILED: Copy not identical";
}
unlink($outputFileName);
?>
===DONE===
--EXPECT--
OK: Copy identical
===DONE===
