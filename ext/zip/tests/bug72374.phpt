--TEST--
Bug #72374 (ZipArchive::addGlob remove_path option strips first char of filename)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';

$dirname = $dirname . 'bug72374/';
mkdir($dirname);
$file = $dirname . 'some-foo.txt';
touch($file);

$zip = new ZipArchive();
$zip->open($dirname . 'test.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addGlob($file, 0, array('add_path' => 'prefix-', 'remove_path' => $dirname . 'some-'));
$zip->addGlob($file, 0, array('add_path' => 'dir-prefix', 'remove_path' => $dirname));
dump_entries_name($zip);
$zip->close();
?>
--CLEAN--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';

$dirname = $dirname . 'bug72374/';
rmdir_rf($dirname);
?>
--EXPECTF--
0 prefix-foo.txt
1 dir-prefix/some-foo.txt
