--TEST--
Bug #81490 (ZipArchive::extractTo() may leak memory)
--SKIPIF--
<?php
if (!extension_loaded("zip")) die("skip zip extension not available");
?>
--FILE--
<?php
$zip = new ZipArchive();
$zip->open(__DIR__ . "/bug81490.zip", ZipArchive::CREATE|ZipArchive::OVERWRITE);
$zip->addFromString("", "yada yada");
mkdir(__DIR__ . "/bug81490");
$zip->open(__DIR__ . "/bug81490.zip");
$zip->extractTo(__DIR__ . "/bug81490", "");
?>
--EXPECT--
--CLEAN--
<?php
@unlink(__DIR__ . "/bug81490.zip");
@rmdir(__DIR__ . "/bug81490");
?>
