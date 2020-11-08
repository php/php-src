--TEST--
ziparchive::addEmptyDir with encoding option
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

include __DIR__ . '/utils.inc';
$dirname = __DIR__ . '/oo_add_encoding_dir/';
$file = $dirname . 'tmp.zip';

@mkdir($dirname);
$zip = new ZipArchive;
if (!$zip->open($file, ZipArchive::CREATE)) {
    exit('failed');
}

$zip->addEmptyDir('foo');
$zip->addEmptyDir(chr(0x82), ZipArchive::FL_ENC_CP437);
$zip->addEmptyDir('è', ZipArchive::FL_ENC_UTF_8);

$zip->addFromString('bar', __FILE__);
$zip->addFromString(chr(0x91), __FILE__, ZipArchive::FL_ENC_CP437);
$zip->addFromString('€', __FILE__, ZipArchive::FL_ENC_UTF_8);

if ($zip->status == ZIPARCHIVE::ER_OK) {
    dump_entries_name($zip);
    $zip->close();
} else {
    echo "failed\n";
}
?>
--CLEAN--
<?php
$dirname = __DIR__ . '/oo_add_encoding_dir/';
unlink($dirname . 'tmp.zip');
rmdir($dirname);
?>
--EXPECT--
0 foo/
1 é/
2 è/
3 bar
4 æ
5 €
