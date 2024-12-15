--TEST--
GH-17137 (Segmentation fault ext/phar/phar.c)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'gh17137.phar';
$phar = new Phar($file);
var_dump($phar, $phar->decompress());
echo "OK\n";
?>
--EXPECTF--
object(Phar)#%d (3) {
  ["pathName":"SplFileInfo":private]=>
  string(0) ""
  ["glob":"DirectoryIterator":private]=>
  bool(false)
  ["subPathName":"RecursiveDirectoryIterator":private]=>
  string(0) ""
}
object(Phar)#%d (3) {
  ["pathName":"SplFileInfo":private]=>
  string(0) ""
  ["glob":"DirectoryIterator":private]=>
  bool(false)
  ["subPathName":"RecursiveDirectoryIterator":private]=>
  string(0) ""
}
OK
