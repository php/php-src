--TEST--
SPL: Problem with casting to string
--SKIPIF--
<?php
if (!defined('GLOB_ERR')) print "skip";
--FILE--
<?php
$d = new DirectoryIterator('.');
var_dump($d);
var_dump(is_string($d));
preg_match('/x/', $d);
var_dump(is_string($d));
?>
--EXPECTF--
object(DirectoryIterator)#%d (4) {
  %s"pathName"%s"SplFileInfo":private]=>
  %s(%d) ".%c%s"
  %s"fileName"%s"SplFileInfo":private]=>
  %s(%d) "%s"
  %s"glob"%s"DirectoryIterator":private]=>
  bool(false)
  %s"subPathName"%s"RecursiveDirectoryIterator":private]=>
  %s(0) ""
}
bool(false)
bool(false)
