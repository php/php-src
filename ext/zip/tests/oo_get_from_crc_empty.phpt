--TEST--
ZipArchive::getFrom*() rejects a CRC-corrupted empty entry
--EXTENSIONS--
zip
--SKIPIF--
<?php
/* libzip < 1.10.0 shortcuts empty entries and never checks their CRC. */
if (version_compare(ZipArchive::LIBZIP_VERSION, '1.10.0', '<')) die('skip libzip < 1.10.0');
?>
--FILE--
<?php
$dirname = __DIR__ . '/oo_get_from_crc_empty_dir';
mkdir($dirname);
$file = $dirname . '/corrupt.zip';

$zip = new ZipArchive();
$zip->open($file, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('empty.txt', '');
$zip->setCompressionName('empty.txt', ZipArchive::CM_STORE);
$zip->close();

/* Corrupt the CRC in both the local and central directory headers. */
$raw = file_get_contents($file);
for ($i = 0, $length = strlen($raw); $i + 3 < $length; $i++) {
    $signature = substr($raw, $i, 4);
    if ($signature === "PK\x03\x04") {
        $raw[$i + 14] = "\x01";
    } elseif ($signature === "PK\x01\x02") {
        $raw[$i + 16] = "\x01";
    }
}
file_put_contents($file, $raw);

$zip = new ZipArchive();
$zip->open($file);
var_dump($zip->getFromName('empty.txt'));
var_dump($zip->getFromIndex(0));
$zip->close();
?>
--CLEAN--
<?php
$dirname = __DIR__ . '/oo_get_from_crc_empty_dir';
@unlink($dirname . '/empty.txt');
@unlink($dirname . '/corrupt.zip');
@rmdir($dirname);
?>
--EXPECTF--
Warning: ZipArchive::getFromName(): Cannot read entry: CRC error in %s on line %d
bool(false)

Warning: ZipArchive::getFromIndex(): Cannot read entry: CRC error in %s on line %d
bool(false)
