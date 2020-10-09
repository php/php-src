--TEST--
Test compress.zlib:// scheme with the file_get_contents
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
$contents = file_get_contents($srcFile);
echo $contents;
?>
--EXPECT--
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
