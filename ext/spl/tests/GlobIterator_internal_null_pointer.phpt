--TEST--
Check Glob iterator is okay with SplFileInfo getPath method calls
--FILE--
<?php

$o = new GlobIterator(__DIR__.'/*.abcdefghij');

echo "Test getATime()\n";
var_dump($o->getATime());
echo "Test getBasename()\n";
var_dump($o->getBasename());
echo "Test getCTime()\n";
var_dump($o->getCTime());
echo "Test getExtension()\n";
var_dump($o->getExtension());
echo "Test getFilename()\n";
var_dump($o->getFilename());
echo "Test getGroup()\n";
var_dump($o->getGroup());
echo "Test getInode()\n";
var_dump($o->getInode());
echo "Test getMTime()\n";
var_dump($o->getMTime());
echo "Test getOwner()\n";
var_dump($o->getOwner());
echo "Test getPath()\n";
var_dump($o->getPath());
echo "Test getPathInfo()\n";
var_dump($o->getPathInfo());
echo "Test getPathname()\n";
var_dump($o->getPathname());
echo "Test getPerms()\n";
var_dump($o->getPerms());
echo "Test getRealPath()\n";
var_dump($o->getRealPath());
echo "Test getSize()\n";
var_dump($o->getSize());
echo "Test getType()\n";
var_dump($o->getType());
echo "Test isDir()\n";
var_dump($o->isDir());
echo "Test isExecutable()\n";
var_dump($o->isExecutable());
echo "Test isFile()\n";
var_dump($o->isFile());
echo "Test isLink()\n";
var_dump($o->isLink());
echo "Test isReadable()\n";
var_dump($o->isReadable());
echo "Test isWritable()\n";
var_dump($o->isWritable());
echo "Test __toString()\n";
var_dump($o->__toString());
echo "Test __debugInfo()\n";
var_dump($o);

?>
--EXPECTF--
Test getATime()
bool(false)
Test getBasename()
string(0) ""
Test getCTime()
bool(false)
Test getExtension()
string(0) ""
Test getFilename()
string(0) ""
Test getGroup()
bool(false)
Test getInode()
bool(false)
Test getMTime()
bool(false)
Test getOwner()
bool(false)
Test getPath()
string(0) ""
Test getPathInfo()
NULL
Test getPathname()
string(0) ""
Test getPerms()
bool(false)
Test getRealPath()
string(%d) "%s"
Test getSize()
bool(false)
Test getType()
bool(false)
Test isDir()
bool(false)
Test isExecutable()
bool(false)
Test isFile()
bool(false)
Test isLink()
bool(false)
Test isReadable()
bool(false)
Test isWritable()
bool(false)
Test __toString()
string(0) ""
Test __debugInfo()
object(GlobIterator)#1 (4) {
  ["pathName":"SplFileInfo":private]=>
  string(0) ""
  ["fileName":"SplFileInfo":private]=>
  string(0) ""
  ["glob":"DirectoryIterator":private]=>
  string(%d) "glob://%s"
  ["subPathName":"RecursiveDirectoryIterator":private]=>
  string(0) ""
}
