--TEST--
Bug #78042 (Incorrect readable and writable flags for php:// streams) size is not allowed
--FILE--
<?php

$file = new \SplTempFileObject(-1);
var_dump($file->getFileInfo()->getPathname());
var_dump($file->valid());
var_dump($file->isReadable());
var_dump($file->isWritable());
var_dump($file->getSize());

--EXPECTF--
string(12) "php://memory"
bool(true)
bool(true)
bool(true)

Fatal error: Uncaught RuntimeException: SplFileInfo::getSize(): stat failed for php://memory in %s
Stack trace:
#0 %s: SplFileInfo->getSize()
#1 {main}
  thrown in %s on line %d
