--TEST--
GH-17808 (PharFileInfo refcount bug)
--EXTENSIONS--
phar
zlib
--FILE--
<?php
$fname = __DIR__.'/tar/files/gh17808.tgz';
copy(__DIR__.'/tar/files/Structures_Graph-1.0.3.tgz', $fname);
$tar = new PharData($fname);
foreach (new RecursiveIteratorIterator($tar) as $file) {
}
var_dump($file);
var_dump(strlen($file->getContent()));
unlink($file);
var_dump($file->getATime());
?>
--CLEAN--
<?php
@unlink(__DIR__.'/tar/files/gh17808.tgz');
?>
--EXPECTF--
object(PharFileInfo)#%d (%d) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "phar://%spackage.xml"
  ["fileName":"SplFileInfo":private]=>
  string(11) "package.xml"
}
int(6747)
int(33188)
