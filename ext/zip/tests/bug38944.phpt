--TEST--
Bug #38944 (newly created ZipArchive segfaults when accessing comment property)
--SKIPIF--
<?php if (!extension_loaded("zip")) print "skip"; ?>
--FILE--
<?php

$arc_name = dirname(__FILE__)."/bug38944.zip";
$foo = new ZipArchive;
$foo->open($arc_name, ZIPARCHIVE::CREATE);;

var_dump($foo->status);
var_dump($foo->statusSys);
var_dump($foo->numFiles);
var_dump($foo->filename);
var_dump($foo->comment);

var_dump($foo);

echo "Done\n";
?>
--EXPECTF--	
int(0)
int(0)
int(0)
string(0) ""
string(0) ""
object(ZipArchive)#1 (5) {
  [u"status"]=>
  int(0)
  [u"statusSys"]=>
  int(0)
  [u"numFiles"]=>
  int(0)
  [u"filename"]=>
  string(0) ""
  [u"comment"]=>
  string(0) ""
}
Done
