<?php
$z = new ZipArchive;
$z->open('test_with_comment.zip');
// Add "Foo Comment" as comment for the foo entry
$z->setCommentName('foo', 'Too Comment ' . time());
$z->close();
