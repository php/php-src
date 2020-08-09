--TEST--
Bug #47667 (ZipArchive::OVERWRITE seems to have no effect)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$thisdir = __DIR__;
$filename = $thisdir . "/bug47667.zip";

$zip = new ZipArchive();
if ($zip->open($filename, ZipArchive::CREATE) !== true) {
    exit("Unable to open the zip file");
} else {
    $zip->addFromString('foo.txt', 'foo bar foobar');
    $zip->close();
}

for ($i = 0; $i < 10; $i++) {
    $zip = new ZipArchive();
    if ($zip->open($filename, ZipArchive::OVERWRITE) !== true) {
        exit("Unable to open the zip file");
    }
    $zip->addFromString("foo_{$i}.txt", 'foo bar foobar');
    $zip->close();
}

$zip = new ZipArchive();
if ($zip->open($filename, ZipArchive::CREATE) !== true) {
    exit("Unable to open the zip file");
}

echo "files: " , $zip->numFiles;
$zip->close();

unlink($filename);
?>
--EXPECT--
files: 1
