--TEST--
Bug #78042 (SplTempFileObject::isReadable() / SplTempFileObject::isWritable() return values)
--FILE--
<?php
$file = new \SplTempFileObject(-1);
$written = $file->fwrite('readable content');
$file->fflush();
$file->rewind();
var_dump($file->getFileInfo()->getPathname());
var_dump($file->valid());
var_dump($file->isReadable());
var_dump($file->fread($written));
var_dump($file->isWritable());
$written += $file->fwrite(' is writable');
$file->fflush();
$file->rewind();
var_dump($file->fread($written));

?>
--EXPECT--
string(12) "php://memory"
bool(true)
bool(true)
string(16) "readable content"
bool(true)
string(28) "readable content is writable"
