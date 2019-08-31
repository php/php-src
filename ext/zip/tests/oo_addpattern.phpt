--TEST--
ZipArchive::addPattern() method
--CREDITS--
Sammy Kaye Powers <sammyk@sammykmedia.com>
w/Kenzo over the shoulder
#phptek Chicago 2014
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
include __DIR__ . '/utils.inc';

$dirname = __DIR__ . '/oo_addpattern_dir/';
$file = $dirname . 'tmp.zip';

@mkdir($dirname);
copy(__DIR__ . '/test.zip', $file);
touch($dirname . 'foo.txt');
touch($dirname . 'bar.txt');

$zip = new ZipArchive();
if (!$zip->open($file)) {
        exit('failed');
}
$dir = realpath($dirname);
$options = array('add_path' => 'baz/', 'remove_path' => $dir);
if (!$zip->addPattern('/\.txt$/', $dir, $options)) {
        echo "failed\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
        if (!verify_entries($zip, [
            "bar",
            "foobar/",
            "foobar/baz",
            "entry1.txt",
            "baz/foo.txt",
            "baz/bar.txt"
        ])) {
            echo "failed\n";
        } else {
            echo "OK";
        }
        $zip->close();
} else {
        echo "failed3\n";
}
?>
--CLEAN--
<?php
$dirname = __DIR__ . '/oo_addpattern_dir/';
unlink($dirname . 'tmp.zip');
unlink($dirname . 'foo.txt');
unlink($dirname . 'bar.txt');
rmdir($dirname);
?>
--EXPECT--
OK
