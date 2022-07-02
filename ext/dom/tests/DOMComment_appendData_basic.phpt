--TEST--
Test adding data to a DOMComment
--CREDITS--
Andrew Larssen <al@larssen.org>
London TestFest 2008
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DomDocument();
$comment = $dom->createComment('test-comment');
$comment->appendData('-more-data');
$dom->appendChild($comment);
$dom->saveXML();
echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<!--test-comment-more-data-->
