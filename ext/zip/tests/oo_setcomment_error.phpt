--TEST--
setComment error behavior
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip zip extension not available');
?>
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

var_dump($zip->setArchiveComment($longComment));
var_dump($zip->setCommentName('entry1.txt', $longComment));
var_dump($zip->setCommentIndex(1, $longComment));

$zip->close();
?>
===DONE===
--EXPECTF--
Warning: ZipArchive::setArchiveComment(): Comment must not exceed 65535 bytes in %s on line %d
bool(false)

Warning: ZipArchive::setCommentName(): Comment must not exceed 65535 bytes in %s on line %d
bool(false)

Warning: ZipArchive::setCommentIndex(): Comment must not exceed 65535 bytes in %s on line %d
bool(false)
===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . '/__tmp_oo_set_comment_error.zip');
?>
