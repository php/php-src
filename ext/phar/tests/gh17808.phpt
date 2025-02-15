--TEST--
GH-17808 (PharFileInfo refcount bug)
--EXTENSIONS--
phar
zlib
--FILE--
<?php
$fname = __DIR__.'/tar/files/Structures_Graph-1.0.3.tgz';
$tar = new PharData($fname);
foreach (new RecursiveIteratorIterator($tar) as $file) {
}
var_dump("$file");
var_dump(strlen($file->getContent()));
unlink("$file");
var_dump($file->getATime());
?>
--EXPECTF--
string(%d) "phar://%spackage.xml"
int(6747)

Warning: unlink(): phar error: "package.xml" in phar %s, has open file pointers, cannot unlink in %s on line %d
int(33188)
