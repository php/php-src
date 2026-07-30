--TEST--
GH-22885 (SEGV when setting the comment of a modified entry)
--EXTENSIONS--
zip
--FILE--
<?php
$file = __DIR__ . '/gh22885.zip';

@unlink($file);

$entries = ['replaced', 'renamed', 'mtime', 'compressed', 'attributes', 'removed', 'cleared', 'untouched'];

$zip = new ZipArchive;
if (!$zip->open($file, ZipArchive::CREATE)) {
    die('failed');
}
foreach ($entries as $entry) {
    $zip->addFromString($entry . '.txt', 'data of ' . $entry);
    $zip->setCommentName($entry . '.txt', 'comment of ' . $entry);
}
$zip->close();

/* The comment of an entry that has been modified cannot be changed: libzip
 * shares it with the copy it made of the central directory record of the entry. */
if (!$zip->open($file)) {
    die('failed');
}

var_dump($zip->addFromString('replaced.txt', 'new data of replaced'));
var_dump($zip->setCommentName('replaced.txt', 'new comment of replaced'));

var_dump($zip->renameName('renamed.txt', 'renamed2.txt'));
var_dump($zip->setCommentName('renamed2.txt', 'new comment of renamed'));

var_dump($zip->setMtimeName('mtime.txt', 1000000));
var_dump($zip->setCommentName('mtime.txt', 'new comment of mtime'));

var_dump($zip->setCompressionName('compressed.txt', ZipArchive::CM_STORE));
var_dump($zip->setCommentName('compressed.txt', 'new comment of compressed'));

var_dump($zip->setExternalAttributesName('attributes.txt', ZipArchive::OPSYS_UNIX, 0100644 << 16));
var_dump($zip->setCommentName('attributes.txt', 'new comment of attributes'));

var_dump($zip->setMtimeName('removed.txt', 1000000));
var_dump($zip->setCommentName('removed.txt', ''));

/* Setting or removing the comment first works, and it survives the modification. */
var_dump($zip->setCommentName('cleared.txt', ''));
var_dump($zip->setMtimeName('cleared.txt', 1000000));

var_dump($zip->setCommentName('untouched.txt', 'new comment of untouched'));
var_dump($zip->setMtimeName('untouched.txt', 1000000));

$zip->close();

if (!$zip->open($file)) {
    die('failed');
}
for ($i = 0; $i < $zip->numFiles; $i++) {
    printf("%s: %s\n", $zip->getNameIndex($i), $zip->getCommentIndex($i));
}
var_dump($zip->getFromName('replaced.txt'));
$zip->close();
?>
--EXPECTF--
bool(true)

Warning: ZipArchive::setCommentName(): Cannot set the comment of an entry that has been modified, set the comment before modifying the entry in %s on line %d
bool(false)
bool(true)

Warning: ZipArchive::setCommentName(): Cannot set the comment of an entry that has been modified, set the comment before modifying the entry in %s on line %d
bool(false)
bool(true)

Warning: ZipArchive::setCommentName(): Cannot set the comment of an entry that has been modified, set the comment before modifying the entry in %s on line %d
bool(false)
bool(true)

Warning: ZipArchive::setCommentName(): Cannot set the comment of an entry that has been modified, set the comment before modifying the entry in %s on line %d
bool(false)
bool(true)

Warning: ZipArchive::setCommentName(): Cannot set the comment of an entry that has been modified, set the comment before modifying the entry in %s on line %d
bool(false)
bool(true)

Warning: ZipArchive::setCommentName(): Cannot set the comment of an entry that has been modified, set the comment before modifying the entry in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
replaced.txt: comment of replaced
renamed2.txt: comment of renamed
mtime.txt: comment of mtime
compressed.txt: comment of compressed
attributes.txt: comment of attributes
removed.txt: comment of removed
cleared.txt: 
untouched.txt: new comment of untouched
string(20) "new data of replaced"
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22885.zip');
?>
