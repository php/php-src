--TEST--
GH-19932 (ZipArchive::setEncryptionName()/setEncryptionIndex() memory leak)
--EXTENSIONS--
zip
--SKIPIF--
<?php if (!method_exists('ZipArchive', 'setEncryptionName')) die('skip encryption not supported'); ?>
--FILE--
<?php
$zip = new ZipArchive();
$zip->open(__DIR__ . "/gh19932.zip", ZipArchive::CREATE);
$zip->addFromString("test.txt", "test");
$zip->setEncryptionName("test.txt", ZipArchive::EM_AES_256, "password");
$zip->setEncryptionName("test.txt", ZipArchive::EM_AES_256, "password");
$zip->setEncryptionIndex("0", ZipArchive::EM_AES_256, "password");
$zip->setEncryptionIndex("0", ZipArchive::EM_AES_256, "password");
$zip->close();
echo "OK";
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh19932.zip");
?>
--EXPECT--
OK

