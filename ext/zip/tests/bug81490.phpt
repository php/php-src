--TEST--
Bug #81490 (ZipArchive::extractTo() may leak memory)
--EXTENSIONS--
zip
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
