--TEST--
SPL: Problem with casting to string (no glob version)
--SKIPIF--
<?php
if (defined('GLOB_ERR')) print "skip GLOB_ERR defined";
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
  %s(%d) ".%c%s"
  %s"fileName"%s"SplFileInfo":private]=>
  %s(%d) "%s"
  %s"subPathName"%s"RecursiveDirectoryIterator":private]=>
  %s(0) ""
}
bool(false)
bool(false)
===DONE===
