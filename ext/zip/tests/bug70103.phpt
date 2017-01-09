--TEST--
Bug #70103 (ZipArchive::addGlob ignores remove_all_path option)
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip zip support not available');
?>
--FILE--
<?php
$dir = __DIR__ . '/bug70103';

mkdir($dir); chmod($dir, 0777);
file_put_contents($dir . '/foo.txt', 'foo');

$zip = new ZipArchive();
$zip->open($dir . '/test.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addGlob($dir . '/*.txt', GLOB_NOSORT, array('remove_all_path' => true));
$zip->close();

$zip = new ZipArchive();
$zip->open($dir . '/test.zip');
var_dump($zip->numFiles);
var_dump($zip->getNameIndex(0));
$zip->close();
?>
--CLEAN--
<?php
$dir = __DIR__ . '/bug70103';
unlink($dir . '/foo.txt');
unlink($dir . '/test.zip');
rmdir($dir);
?>
--EXPECT--
int(1)
string(7) "foo.txt"
