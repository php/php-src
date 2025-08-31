--TEST--
ZipArchive::getArchiveFlag and setArchiveFlag
--EXTENSIONS--
zip
--SKIPIF--
<?php
if(version_compare(ZipArchive::LIBZIP_VERSION, '1.10.0', '<')) die('skip libzip < 1.10.0');?>
--FILE--
<?php
$zip = new ZipArchive();

echo "Open write\n";
$zip->open(__DIR__ . "/test.zip");
var_dump($zip->getArchiveFlag(ZipArchive::AFL_RDONLY));
var_dump($zip->setArchiveFlag(ZipArchive::AFL_RDONLY, 1), $zip->status === ZipArchive::ER_OK);
var_dump($zip->getArchiveFlag(ZipArchive::AFL_RDONLY));
$zip->close();

echo "\nOpen read\n";
$zip->open(__DIR__ . "/test.zip", ZipArchive::RDONLY);
var_dump($zip->getArchiveFlag(ZipArchive::AFL_RDONLY));
var_dump($zip->setArchiveFlag(ZipArchive::AFL_RDONLY, 0), $zip->status !== ZipArchive::ER_OK);
var_dump($zip->getArchiveFlag(ZipArchive::AFL_RDONLY));
$zip->close();


?>
--EXPECTF--
Open write
int(0)
bool(true)
bool(true)
int(1)

Open read
int(1)
bool(false)
bool(true)
int(1)
