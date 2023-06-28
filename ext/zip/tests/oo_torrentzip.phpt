--TEST--
torrentzip format support
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (version_compare(ZipArchive::LIBZIP_VERSION, '1.10.0', '<')) die('skip libzip < 1.10.0');
?>
--FILE--
<?php
$name = __DIR__ . '/torrent.zip';

$zip = new ZipArchive();

echo "Open write\n";
$zip->open($name, ZIPARCHIVE::CREATE);
var_dump($zip->getArchiveFlag(ZipArchive::AFL_IS_TORRENTZIP));
var_dump($zip->setArchiveFlag(ZipArchive::AFL_WANT_TORRENTZIP, 1), $zip->status === ZipArchive::ER_OK);
var_dump($zip->addFile(__FILE__, "test.php"));
$zip->close();

echo "\nOpen read\n";
$zip->open($name, ZipArchive::RDONLY);
var_dump($zip->getArchiveFlag(ZipArchive::AFL_IS_TORRENTZIP));
$zip->close();
?>
--CLEAN--
<?php
$name = __DIR__ . '/torrent.zip';
@unlink($name);
?>
--EXPECTF--
Open write
int(0)
bool(true)
bool(true)
bool(true)

Open read
int(1)
