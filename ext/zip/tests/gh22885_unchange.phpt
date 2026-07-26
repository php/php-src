--TEST--
GH-22885 (unchangeArchive() must not drop the modified entry bookkeeping)
--EXTENSIONS--
zip
--FILE--
<?php
$file = __DIR__ . '/gh22885_unchange.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZipArchive::CREATE)) {
    die('failed');
}
$zip->addFromString('a.txt', 'data of a');
$zip->setCommentName('a.txt', 'comment of a');
$zip->addFromString('b.txt', 'data of b');
$zip->setCommentName('b.txt', 'comment of b');
$zip->setArchiveComment('archive comment');
$zip->close();

if (!$zip->open($file)) {
    die('failed');
}

/* unchangeArchive() only reverts the archive level changes, the entries stay
 * modified, so the comment of a modified entry must still be refused. */
var_dump($zip->setMtimeName('a.txt', 1000000));
var_dump($zip->setArchiveComment('new archive comment'));
var_dump($zip->unchangeArchive());
var_dump($zip->getArchiveComment());
var_dump($zip->setCommentName('a.txt', 'new comment of a'));

/* unchangeName() does revert the entry, so its comment can be set again. */
var_dump($zip->setMtimeName('b.txt', 1000000));
var_dump($zip->unchangeName('b.txt'));
var_dump($zip->setCommentName('b.txt', 'new comment of b'));

$zip->close();

if (!$zip->open($file)) {
    die('failed');
}
var_dump($zip->getArchiveComment());
var_dump($zip->getCommentName('a.txt'));
var_dump($zip->getCommentName('b.txt'));
$zip->close();
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
string(15) "archive comment"

Warning: ZipArchive::setCommentName(): Cannot set the comment of an entry that has been modified, set the comment before modifying the entry in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
string(15) "archive comment"
string(12) "comment of a"
string(16) "new comment of b"
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22885_unchange.zip');
?>
