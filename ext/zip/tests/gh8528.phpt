--TEST--
Bug GH-8528 (ZipArchive::getStream() meta data missing wrapper_type)
--SKIPIF--
<?php
if (!extension_loaded("zip")) die("skip zip extension not available");
?>
--FILE--
<?php
$zip = new ZipArchive();
$zip->open(__DIR__ . "/gh8528.zip", ZipArchive::CREATE|ZipArchive::OVERWRITE);
$zip->addFromString("text.txt", "foo");
$zip->close();

$zip->open(__DIR__ . "/gh8528.zip");
var_dump(stream_get_meta_data($zip->getStream("text.txt"))["wrapper_type"]);
?>
--EXPECT--
string(11) "zip wrapper"
--CLEAN--
<?php
@unlink(__DIR__ . "/gh8528.zip");
?>
