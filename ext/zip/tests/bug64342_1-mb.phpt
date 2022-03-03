--TEST--
Bug #64342 ZipArchive::addFile() has to check file existence (variation 2)
--EXTENSIONS--
zip
--FILE--
<?php

$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . '__私はガラスを食べられますtmp_oo_addfile.zip';

copy($dirname . 'test.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
    exit('failed');
}
if (!$zip->addFile($dirname . 'cant_find_me.txt', 'test.php')) {
    echo "failed\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
    if (!verify_entries($zip, [
        "bar",
        "foobar/",
        "foobar/baz",
        "entry1.txt"
    ])) {
        echo "failed\n";
    } else {
        echo "OK";
    }
    $zip->close();
} else {
    echo "failed\n";
}
@unlink($file);
?>
--EXPECTF--
Warning: ZipArchive::addFile(): No such file or directory in %s on line %d
failed
OK
