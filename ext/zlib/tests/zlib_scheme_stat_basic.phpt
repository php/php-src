--TEST--
Test compress.zlib:// scheme with the unlink function
--EXTENSIONS--
zlib
--FILE--
<?php
$inputFileName = __DIR__."/004.txt.gz";
$srcFile = "compress.zlib://$inputFileName";
stat($srcFile);
lstat($srcFile);
?>
--EXPECTF--
Warning: stat(): stat failed for compress.zlib://%s/004.txt.gz in %s on line %d

Warning: lstat(): Lstat failed for compress.zlib://%s/004.txt.gz in %s on line %d
