--TEST--
Test adding data to a DOMComment
--CREDITS--
Andrew Larssen <al@larssen.org>
London TestFest 2008
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$dom = new DomDocument();
$comment = $dom->createComment('test-comment');
$comment->appendData('-more-data');
$dom->appendChild($comment);
$dom->saveXML();
echo $dom->saveXML();

?>
--EXPECTF--
<?xml version="1.0"?>
<!--test-comment-more-data-->
