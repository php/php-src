--TEST--
Bug #54291 (Crash iterating DirectoryIterator for dir name starting with \0)
--FILE--
<?php
$dir = new DirectoryIterator("\x00/abc");
$dir->isFile();
--EXPECTF--
Fatal error: Uncaught UnexpectedValueException: DirectoryIterator::__construct() expects parameter 1 to be a valid path, string given in %s:%d
Stack trace:
#0 %s(%d): DirectoryIterator->__construct('\x00/abc')
#1 {main}
  thrown in %s on line %d
