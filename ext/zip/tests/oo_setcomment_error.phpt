--TEST--
setComment error behavior
--EXTENSIONS--
zip
--FILE--
<?php
$file = __DIR__ . '/__tmp_oo_set_comment_error.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}

$zip->addFromString('entry1.txt', 'entry #1');
$zip->addFromString('entry2.txt', 'entry #2');

$longComment = str_repeat('a', 0x10000);

try {
    var_dump($zip->setArchiveComment($longComment));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($zip->setCommentName('entry1.txt', $longComment));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($zip->setCommentIndex(1, $longComment));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$zip->close();
?>
--EXPECT--
ZipArchive::setArchiveComment(): Argument #1 ($comment) must be less than 65535 bytes
ZipArchive::setCommentName(): Argument #2 ($comment) must be less than 65535 bytes
ZipArchive::setCommentIndex(): Argument #2 ($comment) must be less than 65535 bytes
--CLEAN--
<?php
@unlink(__DIR__ . '/__tmp_oo_set_comment_error.zip');
?>
