--TEST--
setComment
--EXTENSIONS--
zip
--FILE--
<?php
$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . 'oo_setcomment.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}

$zip->addFromString('entry1.txt', 'entry #1');
$zip->addFromString('entry2.txt', 'entry #2');
$zip->addFromString('dir/entry2d.txt', 'entry #2');
$zip->addFromString('entry4.txt', 'entry #1');
var_dump($zip->setCommentIndex($zip->lastId, 'entry4.txt'));
$zip->addFromString('entry5.txt', 'entry #2');
var_dump($zip->setCommentIndex($zip->lastId, 'entry5.txt'));


var_dump($zip->setCommentName('entry1.txt', 'entry1.txt'));
var_dump($zip->setCommentName('entry2.txt', 'entry2.txt'));
var_dump($zip->setCommentName('dir/entry2d.txt', 'dir/entry2d.txt'));
var_dump($zip->setArchiveComment('archive'));

var_dump($zip->setArchiveComment('archive'));

if (!$zip->status == ZIPARCHIVE::ER_OK) {
    echo "failed to write zip\n";
}
$zip->close();

if (!$zip->open($file)) {
    @unlink($file);
    exit('failed');
}

var_dump($zip->getCommentIndex(0));
var_dump($zip->getCommentIndex(1));
var_dump($zip->getCommentIndex(2));
var_dump($zip->getCommentIndex(3));
var_dump($zip->getCommentIndex(4));
var_dump($zip->getArchiveComment());

$zip->close();
@unlink($file);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(10) "entry1.txt"
string(10) "entry2.txt"
string(15) "dir/entry2d.txt"
string(10) "entry4.txt"
string(10) "entry5.txt"
string(7) "archive"
