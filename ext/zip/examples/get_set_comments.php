<?php
error_reporting(E_ALL|E_STRICT);

copy('test_with_comment.zip', 't.zip');
$z = new ZipArchive;
$z->open('t.zip');

print_r($z);

for ($i=0; $i<$z->numFiles; $i++) {
	echo "index: $i\n";
	print_r($z->getCommentIndex($i));
	echo "\n\n";
}
echo "foobar/ " . $z->getCommentName('foobar/') . "\n";

echo "Archive comment: " . $z->getArchiveComment() . "\n";


$z->setCommentIndex(1, 'new comment idx 1');
$z->setCommentName('foobar/', 'new comment foobar/');

$z->setArchiveComment( 'new archive comment');

for ($i=0; $i<$z->numFiles; $i++) {
	echo "index: $i\n";
	print_r($z->getCommentIndex($i));
	echo "\n\n";
}

echo $z->getCommentName('foobar/') . "\n";

// Get the original comment
echo $z->getCommentName('foobar/', ZIPARCHIVE::FL_UNCHANGED) . "\n";

echo "Archive comment: " . $z->getArchiveComment() . "\n";
echo "Archive comment (original): " . $z->getArchiveComment(ZIPARCHIVE::FL_UNCHANGED) . "\n";
