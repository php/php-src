--TEST--
Test basic ZipArchive::unchangeIndex() Method
--CREDITS--
PHP TestFest 2017 - Bergfreunde, Florian Engelhardt
--EXTENSIONS--
zip
--FILE--
<?php
$dirname = __DIR__ . '/';
$file = $dirname . 'oo_unchangeIndex.zip';
copy($dirname.'test.zip', $file);

var_dump(md5_file($file));

$zip = new ZipArchive();
$zip->open($file);
var_dump($zip->getNameIndex(0));
var_dump($zip->getCommentIndex(0));

$zip->renameIndex(0, 'baz filename');
$zip->setCommentIndex(0, 'baz comment');

var_dump($zip->getNameIndex(0));
var_dump($zip->getCommentIndex(0));

$zip->unchangeIndex(0);

var_dump($zip->getNameIndex(0));
var_dump($zip->getCommentIndex(0));

$zip->close();

var_dump(md5_file($file));
?>
--CLEAN--
<?php
unlink(__DIR__.'/oo_unchangeIndex.zip');
?>
--EXPECT--
string(32) "cb753d0a812b2edb386bdcbc4cd7d131"
string(3) "bar"
string(0) ""
string(12) "baz filename"
string(11) "baz comment"
string(3) "bar"
string(0) ""
string(32) "cb753d0a812b2edb386bdcbc4cd7d131"
