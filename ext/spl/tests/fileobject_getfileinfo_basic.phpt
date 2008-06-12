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
?>
--EXPECTF--
object(SplFileInfo)#2 (2) {
  [u"pathName":u"SplFileInfo":private]=>
  %s(%d) "%sext%espl%etests"
  [u"fileName":u"SplFileInfo":private]=>
  %s(%d) "%sext%espl%etests%efileobject_getfileinfo_basic.php"
}
%s(%d) "%sext%espl%etests%efileobject_getfileinfo_basic.php"

object(SplFileInfo)#4 (2) {
  [u"pathName":u"SplFileInfo":private]=>
  %s(%d) "%sext%espl"
  [u"fileName":u"SplFileInfo":private]=>
  %s(%d) "%sext%espl%etests"
}
%s(%d) "%sext%espl%etests"
