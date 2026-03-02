--TEST--
Phar - bug #70433 - Uninitialized pointer in phar_make_dirstream when zip entry filename is "/"
--EXTENSIONS--
phar
zlib
--FILE--
<?php
$phar = new PharData(__DIR__."/bug70433.zip");
var_dump($phar);
$meta = $phar->getMetadata();
var_dump($meta);
?>
DONE
--EXPECT--
object(PharData)#1 (3) {
  ["pathName":"SplFileInfo":private]=>
  string(0) ""
  ["glob":"DirectoryIterator":private]=>
  bool(false)
  ["subPathName":"RecursiveDirectoryIterator":private]=>
  string(0) ""
}
NULL
DONE
