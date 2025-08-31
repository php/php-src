--TEST--
ziparchive::addFile() function
--EXTENSIONS--
zip
--FILE--
<?php

$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'oo_addfile.zip';

copy($dirname . 'test.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
    exit('failed');
}
var_dump($zip->lastId);
if (!$zip->addFile($dirname . 'utils.inc', 'test.php')) {
    echo "failed\n";
}
var_dump($zip->lastId);
if (!$zip->addFile($dirname . 'utils.inc', 'mini.txt', 12, 34)) {
    echo "failed\n";
}
var_dump($zip->lastId);
if (!$zip->addFile($dirname . 'utils.inc', 'other.txt', 0, ZipArchive::LENGTH_TO_END, ZipArchive::FL_OPEN_FILE_NOW)) {
    echo "failed\n";
}
var_dump($zip->lastId);
$del = $dirname . '__tmp_oo_addfile.txt';
file_put_contents($del, 'foo');
if (!$zip->addFile($del, 'deleted.txt', 0, 0, ZipArchive::FL_OPEN_FILE_NOW)) {
    echo "failed\n";
}
if (substr(PHP_OS, 0, 3) != 'WIN') {
	unlink($del);
}
var_dump($zip->lastId);
if ($zip->status == ZIPARCHIVE::ER_OK) {
    if (!verify_entries($zip, [
        "bar",
        "foobar/",
        "foobar/baz",
        "entry1.txt",
        "test.php",
        "mini.txt",
        "other.txt",
        "deleted.txt"
    ])) {
        echo "failed\n";
    } else {
        echo "OK\n";
    }
    $zip->close();
} else {
    echo "failed\n";
}
if (!$zip->open($file)) {
    exit('failed');
}
var_dump(strlen($zip->getFromName('test.php')) == filesize($dirname . 'utils.inc'));
var_dump(strlen($zip->getFromName('mini.txt')) == 34);
var_dump(strlen($zip->getFromName('other.txt')) == filesize($dirname . 'utils.inc'));
var_dump($zip->getFromName('deleted.txt') === "foo");
@unlink($file);
@unlink($del);
?>
--EXPECT--
int(-1)
int(4)
int(5)
int(6)
int(7)
OK
bool(true)
bool(true)
bool(true)
bool(true)
