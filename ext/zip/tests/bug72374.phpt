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
$zip->addGlob($file, 0, array('remove_path' => $dirname . 'some-'));
$zip->addGlob($file, 0, array('remove_path' => $dirname));
verify_entries($zip, ['foo.txt', '/some-foo.txt']);
$zip->close();
?>
--CLEAN--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';

$dirname = $dirname . 'bug72374/';
rmdir_rf($dirname);
?>
--EXPECT--
