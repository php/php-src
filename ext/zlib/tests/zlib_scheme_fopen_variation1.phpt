--TEST--
Test compress.zlib:// scheme with the fopen on a file scheme
--EXTENSIONS--
zlib
--FILE--
<?php
$inputFileName = __DIR__."/004.txt.gz";
$srcFile = "file://$inputFileName";
$compressedFile = "compress.zlib://$srcFile";

echo "file=$compressedFile\n\n";
$h = fopen($compressedFile, 'r');
fpassthru($h);
fclose($h);
?>
--EXPECTF--
file=compress.zlib://file://%s/004.txt.gz

When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
