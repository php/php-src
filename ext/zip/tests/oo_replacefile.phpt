--TEST--
ziparchive::replaceFile() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'oo_replacefile.zip';

copy($dirname . 'test.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
    exit('open failed');
}
if ($zip->replaceFile($dirname . 'utils.inc', 5)) {
    echo "replace 5 succeed\n";
}
if (!$zip->replaceFile($dirname . 'utils.inc', 0)) {
    echo "replace 0 failed\n";
}
if (!$zip->replaceFile($dirname . 'utils.inc', 2, 12, 42)) {
    echo "replace 2 failed\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
    if (!verify_entries($zip, [
        "bar",
        "foobar/",
        "foobar/baz",
        "entry1.txt",
    ])) {
        echo "verify failed\n";
    } else {
        echo "OK\n";
    }
    $zip->close();
} else {
    echo "failed\n";
}
if (!$zip->open($file)) {
    exit('re-open failed');
}
var_dump(strlen($zip->getFromName('bar')) == filesize($dirname . 'utils.inc'));
var_dump(strlen($zip->getFromName('foobar/baz')) == 42);
@unlink($file);
?>
--EXPECT--
OK
bool(true)
bool(true)
