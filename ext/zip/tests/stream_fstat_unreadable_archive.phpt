--TEST--
fstat() on a zip:// stream whose archive can no longer be opened
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip the archive cannot be rewritten while it is open');
?>
--FILE--
<?php
$file = __DIR__ . '/stream_fstat_unreadable_archive.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZipArchive::CREATE)) {
    exit('failed');
}

$zip->addFromString('entry.txt', 'entry');
$zip->close();

$fp = fopen('zip://' . $file . '#entry.txt', 'rb');
var_dump($fp !== false);

file_put_contents($file, 'this is not a zip archive');

var_dump(fstat($fp));

fclose($fp);
?>
--EXPECT--
bool(true)
bool(false)
--CLEAN--
<?php
unlink(__DIR__ . '/stream_fstat_unreadable_archive.zip');
?>
