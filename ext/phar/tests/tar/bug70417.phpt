--TEST--
Bug #70417 (PharData::compress() doesn't close temp file)
--EXTENSIONS--
phar
zlib
--FILE--
<?php
$filename = __DIR__ . '/bug70417.tar';
@unlink("$filename.gz");
$resBefore = count(get_resources());
$arch = new PharData($filename);
$arch->addFromString('foo', 'bar');
$arch->compress(Phar::GZ);
unset($arch);
$resAfter = count(get_resources());
var_dump($resBefore === $resAfter);
?>
--CLEAN--
<?php
$filename = __DIR__ . '/bug70417.tar';
@unlink($filename);
@unlink("$filename.gz");
?>
--EXPECT--
bool(true)
