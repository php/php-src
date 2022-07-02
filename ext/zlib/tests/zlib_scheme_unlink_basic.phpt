--TEST--
Test compress.zlib:// scheme with the unlink function
--EXTENSIONS--
zlib
--FILE--
<?php
$inputFileName = __DIR__."/004.txt.gz";
$srcFile = "compress.zlib://$inputFileName";
unlink($srcFile);
var_dump(file_exists($inputFileName));
?>
--EXPECTF--
Warning: unlink(): ZLIB does not allow unlinking in %s on line %d
bool(true)
