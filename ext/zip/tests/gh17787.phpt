--TEST--
GH-17787 (ZipArchive stream stops reading early when the archive is freed while the stream is open)
--EXTENSIONS--
zip
--FILE--
<?php
$name = __DIR__ . '/gh17787.zip';
$data = str_repeat("The quick brown fox jumps over the lazy dog.\n", 4000);

$zip = new ZipArchive;
$zip->open($name, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('entry.txt', $data);
$zip->close();

$zip = new ZipArchive;
$zip->open($name, ZipArchive::RDONLY);
$stream = $zip->getStreamIndex(0, ZipArchive::FL_UNCHANGED);

// Free the archive while the stream is still open
$zip = null;

var_dump(stream_get_contents($stream) === $data);
fclose($stream);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh17787.zip');
?>
--EXPECT--
bool(true)
