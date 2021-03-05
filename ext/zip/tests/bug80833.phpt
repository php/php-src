--TEST--
Bug #80833 (ZipArchive::getStream doesn't use setPassword)
--SKIPIF--
<?php
if (!extension_loaded('zip')) die("skip zip extension not available");
?>
--FILE--
<?php
$create_zip = new ZipArchive();
$create_zip->open(__DIR__ . "/80833.zip", ZipArchive::CREATE);
$create_zip->setPassword("default_password");
$create_zip->addFromString("test.txt", "This is a test string.");
$create_zip->setEncryptionName("test.txt", ZipArchive::EM_AES_256, "file_password");
$create_zip->close();

$extract_zip = new ZipArchive();
$extract_zip->open(__DIR__ . "/80833.zip", ZipArchive::RDONLY);
$extract_zip->setPassword("default_password");
$file_stream = $extract_zip->getStream("test.txt", "file_password");
var_dump(stream_get_contents($file_stream));
fclose($file_stream);
$extract_zip->close();
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/80833.zip");
?>
--EXPECT--
string(22) "This is a test string."
