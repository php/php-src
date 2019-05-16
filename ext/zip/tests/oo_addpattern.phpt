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
        dump_entries_name($zip);
        $zip->close();
} else {
        echo "failed\n";
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
0 bar
1 foobar/
2 foobar/baz
3 entry1.txt
4 baz/bar.txt
5 baz/foo.txt
