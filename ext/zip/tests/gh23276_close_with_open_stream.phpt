--TEST--
GH-23276 (ZipArchive dropping its archive while a stream is open leaves the object collectable)
--EXTENSIONS--
zip
--FILE--
<?php
$filename = __DIR__ . '/gh23276_close_with_open_stream.zip';

$zip = new ZipArchive;
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('entry.txt', 'contents');
$zip->close();

$zip->open($filename, ZipArchive::RDONLY);
$stream = $zip->getStream('entry.txt');
var_dump($zip->close());
$weakRef = WeakReference::create($zip);
unset($zip);
var_dump($weakRef->get());
var_dump(stream_get_contents($stream));
fclose($stream);

$zip = new ZipArchive;
$zip->open($filename, ZipArchive::RDONLY);
$stream = $zip->getStream('entry.txt');
var_dump($zip->open($filename, ZipArchive::RDONLY));
$weakRef = WeakReference::create($zip);
unset($zip);
var_dump($weakRef->get());
var_dump(stream_get_contents($stream));
fclose($stream);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23276_close_with_open_stream.zip');
?>
--EXPECTF--
bool(true)
NULL

Warning: stream_get_contents(): Zip stream error: %s in %s on line %d
string(0) ""
bool(true)
NULL

Warning: stream_get_contents(): Zip stream error: %s in %s on line %d
string(0) ""
