--TEST--
Test compress.zlib:// scheme with the file
--EXTENSIONS--
zlib
--FILE--
<?php
$inputFileName = __DIR__."/004.txt.gz";
$srcFile = "compress.zlib://$inputFileName";
$contents = file($srcFile);
var_dump($contents);
?>
--EXPECT--
array(6) {
  [0]=>
  string(36) "When you're taught through feelings
"
  [1]=>
  string(26) "Destiny flying high above
"
  [2]=>
  string(38) "all I know is that you can realize it
"
  [3]=>
  string(18) "Destiny who cares
"
  [4]=>
  string(19) "as it turns around
"
  [5]=>
  string(39) "and I know that it descends down on me
"
}
