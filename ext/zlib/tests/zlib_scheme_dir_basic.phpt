--TEST--
Test compress.zlib:// scheme with the directory functions
--EXTENSIONS--
zlib
--FILE--
<?php
$inputFileName = __DIR__."/dir.gz";
$srcFile = "compress.zlib://$inputFileName";
var_dump(mkdir($srcFile));
var_dump(is_dir($srcFile));
var_dump(opendir($srcFile));
var_dump(rmdir($srcFile));
?>
--EXPECTF--
bool(false)
bool(false)

Warning: opendir(compress.zlib://%s/dir.gz): Failed to open directory: not implemented in %s on line %d
bool(false)
bool(false)
