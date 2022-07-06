--TEST--
Bug #38944 (newly created ZipArchive segfaults when accessing comment property)
--SKIPIF--
<?php if (!extension_loaded("zip")) print "skip"; ?>
--FILE--
<?php

$arc_name = __DIR__."/bug38944.zip";
$foo = new ZipArchive;
$foo->open($arc_name, ZIPARCHIVE::CREATE);

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
string(%d) "%s"
string(0) ""
object(ZipArchive)#%d (6) {
  ["lastId"]=>
  int(-1)
  ["status"]=>
  int(0)
  ["statusSys"]=>
  int(0)
  ["numFiles"]=>
  int(0)
  ["filename"]=>
  string(%d) "%s"
  ["comment"]=>
  string(0) ""
}
Done
