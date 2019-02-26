--TEST--
ZipArchive::addGlob() method
--CREDITS--
Sammy Kaye Powers <sammyk@sammykmedia.com>
w/Kenzo over the shoulder
#phptek Chicago 2014
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
if(!defined("GLOB_BRACE")) die ('skip');
?>
--FILE--
<?php
include __DIR__ . '/utils.inc';
$dirname = __DIR__ . '/oo_addglob_dir/';
$file = $dirname . 'tmp.zip';

@mkdir($dirname);
copy(__DIR__ . '/test.zip', $file);
touch($dirname . 'foo.txt');
touch($dirname . 'bar.baz');

$zip = new ZipArchive();
if (!$zip->open($file)) {
        exit('failed');
}
$options = array('add_path' => 'baz/', 'remove_all_path' => TRUE);
if (!$zip->addGlob($dirname . '*.{txt,baz}', GLOB_BRACE, $options)) {
        echo "failed1\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
        dump_entries_name($zip);
        $zip->close();
} else {
        echo "failed2\n";
}
?>
--CLEAN--
<?php
$dirname = __DIR__ . '/oo_addglob_dir/';
unlink($dirname . 'tmp.zip');
unlink($dirname . 'foo.txt');
unlink($dirname . 'bar.baz');
rmdir($dirname);
?>
--EXPECT--
0 bar
1 foobar/
2 foobar/baz
3 entry1.txt
4 baz/foo.txt
5 baz/bar.baz
