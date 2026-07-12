--TEST--
GH-22649 (setCommentName/Index after addFromString should not segfault)
--EXTENSIONS--
zip
--FILE--
<?php
$file = __DIR__ . '/gh22649.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZipArchive::CREATE)) {
    exit('failed');
}

$zip->addFromString('dir/entry2d.txt', 'entry #2');
var_dump($zip->setCommentName('dir/entry2d.txt', 'dir/entry2d.txt'));

$zip->addFromString('dir/entry3d.txt', 'entry #3');
var_dump($zip->setCommentIndex($zip->lastId, 'dir/entry3d.txt'));

$zip->close();

if (!$zip->open($file, ZipArchive::CREATE)) {
    exit('failed');
}

$zip->addFromString('dir/entry2d.txt', 'updated entry #2');
var_dump($zip->setCommentName('dir/entry2d.txt', 'dir/entry2d.txt'));

$zip->addFromString('dir/entry3d.txt', 'updated entry #3');
var_dump($zip->setCommentIndex($zip->lastId, 'dir/entry3d.txt'));

$zip->close();

if (!$zip->open($file)) {
    exit('failed');
}

var_dump($zip->getCommentName('dir/entry2d.txt'));
var_dump($zip->getCommentName('dir/entry3d.txt'));

$zip->close();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(15) "dir/entry2d.txt"
string(15) "dir/entry3d.txt"
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22649.zip');
?>
