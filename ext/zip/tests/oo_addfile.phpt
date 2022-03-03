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
if ($zip->status == ZIPARCHIVE::ER_OK) {
    if (!verify_entries($zip, [
        "bar",
        "foobar/",
        "foobar/baz",
        "entry1.txt",
        "test.php",
        "mini.txt"
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
@unlink($file);
?>
--EXPECT--
int(-1)
int(4)
int(5)
OK
bool(true)
bool(true)
