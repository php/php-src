--TEST--
getComment
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = __DIR__ . '/';
$file = $dirname . 'test_with_comment.zip';
include $dirname . 'utils.inc';
$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}
echo $zip->getArchiveComment() . "\n";

$idx = $zip->locateName('foo');
echo $zip->getCommentName('foo') . "\n";
echo $zip->getCommentIndex($idx);

echo $zip->getCommentName('') . "\n";
echo $zip->getCommentName() . "\n";

$zip->close();

?>
--EXPECTF--
Zip archive comment
foo comment
foo comment
Notice: ZipArchive::getCommentName(): Empty string as entry name in %s on line %d


Warning: ZipArchive::getCommentName() expects at least 1 parameter, 0 given in %s on line %d
