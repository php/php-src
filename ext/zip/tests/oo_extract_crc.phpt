--TEST--
ZipArchive::extractTo() and getFrom*() fail on a CRC-corrupted entry
--EXTENSIONS--
zip
--FILE--
<?php
$dirname = __DIR__ . '/oo_extract_crc_dir';
mkdir($dirname);
$file = $dirname . '/corrupt.zip';
$payload = str_repeat('A', 64) . 'PAYLOAD-END';

$zip = new ZipArchive();
$zip->open($file, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('a.txt', $payload);
$zip->setCompressionName('a.txt', ZipArchive::CM_STORE);
$zip->close();

$raw = file_get_contents($file);
$raw[strpos($raw, 'AAAA') + 2] = 'Z';
file_put_contents($file, $raw);

$zip = new ZipArchive();
$zip->open($file);
var_dump($zip->extractTo($dirname, 'a.txt'));
var_dump($zip->getFromName('a.txt'));
var_dump($zip->getFromIndex(0));
$zip->close();
?>
--CLEAN--
<?php
$dirname = __DIR__ . '/oo_extract_crc_dir';
@unlink($dirname . '/a.txt');
@unlink($dirname . '/corrupt.zip');
@rmdir($dirname);
?>
--EXPECTF--
Warning: ZipArchive::extractTo(): Cannot extract "a.txt": "CRC error" in %s on line %d
bool(false)

Warning: ZipArchive::getFromName(): Cannot read entry: CRC error in %s on line %d
bool(false)

Warning: ZipArchive::getFromIndex(): Cannot read entry: CRC error in %s on line %d
bool(false)
