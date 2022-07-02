--TEST--
ziparchive::addEmptyDir
--EXTENSIONS--
zip
--FILE--
<?php

$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'oo_addemptydir.zip';

copy($dirname . 'test.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
    exit('failed');
}

var_dump($zip->lastId); // -1 (nothing added)
$zip->addEmptyDir('emptydir');
var_dump($zip->lastId); // 4
$zip->addEmptyDir('emptydir');
var_dump($zip->lastId); // -1 (already exists)
$zip->addEmptyDir('emptydir', ZipArchive::FL_OVERWRITE);
var_dump($zip->lastId); // 4
if ($zip->status == ZIPARCHIVE::ER_OK) {
    if (!verify_entries($zip, [
        "bar",
        "foobar/",
        "foobar/baz",
        "entry1.txt",
        "emptydir/"
    ])) {
        echo "failed\n";
    } else {
        echo "OK";
    }
    $zip->close();
} else {
    echo "failed3\n";
}
@unlink($file);
?>
--EXPECT--
int(-1)
int(4)
int(-1)
int(4)
OK
