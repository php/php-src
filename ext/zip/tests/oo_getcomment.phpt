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
var_dump($zip->getCommentName('foo'));
var_dump($zip->getCommentIndex($idx));

try {
    echo $zip->getCommentName('') . "\n";
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$zip->close();

?>
--EXPECT--
Zip archive comment
string(11) "foo comment"
string(11) "foo comment"
ZipArchive::getCommentName(): Argument #1 ($name) cannot be empty
