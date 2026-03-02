--TEST--
GH-18953 (Phar: Stream double free)
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
--FILE--
<?php

$phar = new Phar(__DIR__ . "/gh18953.phar");
$phar->addFromString("file", str_repeat("123", random_int(1, 1)));
$phar->addEmptyDir("dir");
$phar2 = $phar->compress(Phar::GZ);

var_dump($phar["dir"]);
var_dump($phar2["dir"]);
var_dump($phar["file"]->openFile()->fread(100));
var_dump($phar2["file"]->openFile()->fread(100));

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh18953.phar");
@unlink(__DIR__ . "/gh18953.phar.gz");
?>
--EXPECTF--
object(PharFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%sphar%sdir"
  ["fileName":"SplFileInfo":private]=>
  string(3) "dir"
}
object(PharFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%sphar.gz%sdir"
  ["fileName":"SplFileInfo":private]=>
  string(3) "dir"
}
string(3) "123"
string(3) "123"
