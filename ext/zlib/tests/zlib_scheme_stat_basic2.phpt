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
echo "file_exists=";
var_dump(file_exists($srcFile));
echo "is_file=";
var_dump(is_file($srcFile));
echo "is_dir=";
var_dump(is_dir($srcFile));
echo "is_readable=";
var_dump(is_readable($srcFile));
echo "\n";
echo "filesize=";
var_dump(filesize($srcFile));
echo "filetype=";
var_dump(filetype($srcFile));
echo "fileatime=";
var_dump(fileatime($srcFile));

?>
===DONE===
--EXPECTF--
file_exists=bool(false)
is_file=bool(false)
is_dir=bool(false)
is_readable=bool(false)

filesize=
Warning: filesize(): stat failed for compress.zlib://%s004.txt.gz in %s on line %d
bool(false)
filetype=
Warning: filetype(): Lstat failed for compress.zlib://%s004.txt.gz in %s on line %d
bool(false)
fileatime=
Warning: fileatime(): stat failed for compress.zlib://%s004.txt.gz in %s on line %d
bool(false)
===DONE===
