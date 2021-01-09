--TEST--
Bug #78042 (Incorrect readable and writable flags for php:// streams)
--FILE--
<?php

$file = new \SplTempFileObject(-1);
var_dump($file->getFileInfo()->getPathname());
var_dump($file->valid());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://memory");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://stdin");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://stdout");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://stderr");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://input");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://output");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://temp");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

$file = new \SplFileInfo("php://fd/1");
var_dump($file->getFileInfo()->getPathname());
var_dump($file->isReadable());
var_dump($file->isWritable());

--EXPECT--
string(12) "php://memory"
bool(true)
bool(true)
bool(true)
string(12) "php://memory"
bool(true)
bool(true)
string(11) "php://stdin"
bool(true)
bool(false)
string(12) "php://stdout"
bool(false)
bool(true)
string(12) "php://stderr"
bool(false)
bool(true)
string(11) "php://input"
bool(true)
bool(false)
string(12) "php://output"
bool(false)
bool(true)
string(10) "php://temp"
bool(true)
bool(true)
string(10) "php://fd/1"
bool(false)
bool(false)