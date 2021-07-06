--TEST--
Bug #69279 (Compressed ZIP Phar extractTo() creates garbage files)
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
--FILE--
<?php
$w = new Phar(__DIR__ . "/bug69279.phar.zip");
$w["bug69279.txt"] = "Sample content.";
$w->compressFiles(Phar::GZ);
unset($w);

$r = new Phar(__DIR__ . "/bug69279.phar.zip");
var_dump($r["bug69279.txt"]->isCompressed());

$r->extractTo(__DIR__, NULL, TRUE);
var_dump(file_get_contents(__DIR__ . "/bug69279.txt"));
?>
--EXPECT--
bool(true)
string(15) "Sample content."
--CLEAN--
<?php
@unlink(__DIR__ . "/bug69279.txt");
@unlink(__DIR__ . "/bug69279.phar.zip");
?>
