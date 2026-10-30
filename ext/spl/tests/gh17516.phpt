--TEST--
GH-17516 SplTempFileObject::getPathInfo() crashes on invalid class ID.
--FILE--
<?php
$cls = new SplTempFileObject();
class SplFileInfoChild extends SplFileInfo {}
class BadSplFileInfo {}

var_dump($cls->getPathInfo('SplFileInfoChild'));

try {
	$cls->getPathInfo('BadSplFileInfo');
} catch (\TypeError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
object(SplFileInfoChild)#2 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(4) "php:"
  ["fileName":"SplFileInfo":private]=>
  string(4) "php:"
}
SplFileInfo::getPathInfo(): Argument #1 ($class) must be a class name derived from SplFileInfo or null, BadSplFileInfo given
