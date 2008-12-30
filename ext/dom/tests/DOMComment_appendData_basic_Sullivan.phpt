--TEST--
DOMComment::appendData basic functionality test
--CREDITS--
Mike Sullivan <mike@regexia.com>
#TestFest 2008 (London)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$comment = $document->createElement('comment');
$root->appendChild($comment);

$commentnode = $document->createComment('');
$comment->appendChild($commentnode);
$commentnode->appendData('data');
echo "Comment Length (one append): " . $commentnode->length . "\n";

$commentnode->appendData('><&"');
echo "Comment Length (two appends): " . $commentnode->length . "\n";

echo "Comment Content: " . $commentnode->data . "\n";

echo "\n" . $document->saveXML();

?>
--EXPECT--
Comment Length (one append): 4
Comment Length (two appends): 8
Comment Content: data><&"

<?xml version="1.0"?>
<root><comment><!--data><&"--></comment></root>