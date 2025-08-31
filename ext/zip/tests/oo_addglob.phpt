--TEST--
ZipArchive::addGlob() method
--CREDITS--
Sammy Kaye Powers <sammyk@sammykmedia.com>
w/Kenzo over the shoulder
#phptek Chicago 2014
--EXTENSIONS--
zip
--SKIPIF--
<?php
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
    echo "failed 1\n";
}
if (!$zip->addGlob($dirname . '*.{txt,baz}', GLOB_BRACE, $options)) {
    echo "failed 2\n";
}
$options['flags'] = 0; // clean FL_OVERWRITE
if (!$zip->addGlob($dirname . '*.{txt,baz}', GLOB_BRACE, $options)) {
    var_dump($zip->getStatusString());
}
$options['flags'] = ZipArchive::FL_OVERWRITE;
if (!$zip->addGlob($dirname . '*.{txt,baz}', GLOB_BRACE, $options)) {
    echo "failed 3\n";
}
if ($zip->status == ZIPARCHIVE::ER_OK) {
        if (!verify_entries($zip, [
            "bar",
            "foobar/",
            "foobar/baz",
            "entry1.txt",
            "baz/foo.txt",
            "baz/bar.baz"
        ])) {
            echo "failed\n";
        } else {
            echo "OK";
        }
        $zip->close();
} else {
        echo "failed 4\n";
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
string(19) "File already exists"
OK
