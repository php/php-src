--TEST--
SPL: SplFileObject::getFileInfo
--CREDITS--
H�vard Eide <nucleuz at gmail.com>
#Testfest php.no
--INI--
include_path=.
--FILE--
<?php
$file = __FILE__;
$s = new SplFileObject( $file );
var_dump($fi = $s->getFileInfo(), (string)$fi);

$d = new SplFileInfo( __DIR__ );
echo "\n";
var_dump($fi = $d->getFileInfo(), (string)$fi);
$d = new SplFileInfo( __DIR__."/" );
echo "\n";
var_dump($fi = $d->getFileInfo(), (string)$fi);
?>
--EXPECTF--
object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%sext%espl%etests%eSplFileObject%efileobject_getfileinfo_basic.php"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "fileobject_getfileinfo_basic.php"
}
string(%d) "%sext%espl%etests%eSplFileObject%efileobject_getfileinfo_basic.php"

object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%sext%e%spl%etests%eSplFileObject"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "SplFileObject"
}
string(%d) "%sext%espl%etests%eSplFileObject"

object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%sext%espl%etests%eSplFileObject"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "SplFileObject"
}
string(%d) "%sext%espl%etests%eSplFileObject"
