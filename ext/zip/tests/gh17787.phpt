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

// Same with getStreamName()
$zip = new ZipArchive;
$zip->open($name, ZipArchive::RDONLY);
$stream = $zip->getStreamName('entry.txt', ZipArchive::FL_UNCHANGED);
$zip = null;

var_dump(stream_get_contents($stream) === $data);
fclose($stream);

// Same with getStream()
$zip = new ZipArchive;
$zip->open($name, ZipArchive::RDONLY);
$stream = $zip->getStream('entry.txt');
$zip = null;

var_dump(stream_get_contents($stream) === $data);
fclose($stream);

// Pending changes are still committed once the last stream is closed
$name = __DIR__ . '/gh17787_write.zip';

$zip = new ZipArchive;
var_dump($zip->open($name, ZipArchive::CREATE | ZipArchive::OVERWRITE));
$zip->addFromString('first.txt', 'first');
$zip->close();

$zip = new ZipArchive;
var_dump($zip->open($name));
$zip->addFromString('second.txt', 'second');
$stream = $zip->getStreamName('first.txt', ZipArchive::FL_UNCHANGED);
$zip = null;

var_dump(stream_get_contents($stream));
fclose($stream);

$zip = new ZipArchive;
var_dump($zip->open($name, ZipArchive::RDONLY));
var_dump($zip->numFiles);
var_dump($zip->getFromName('second.txt'));
$zip->close();
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh17787.zip');
@unlink(__DIR__ . '/gh17787_write.zip');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(5) "first"
bool(true)
int(2)
string(6) "second"
