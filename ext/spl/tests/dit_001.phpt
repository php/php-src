--TEST--
SPL: Problem with casting to string
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
$d = new DirectoryIterator('.');
var_dump($d);
var_dump(is_string($d));
preg_match('/x/', $d);
var_dump(is_string($d));
?>
===DONE===
--EXPECTF--
object(DirectoryIterator)#%d (3) {
  %s"pathName"%s"SplFileInfo":private]=>
  %s(1) "."
  %s"glob"%s"RecursiveDirectoryIterator":private]=>
  bool(false)
  %s"subPathName"%s"RecursiveDirectoryIterator":private]=>
  %s(0) ""
}
bool(false)
bool(false)
===DONE===
