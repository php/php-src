--TEST--
Test replacing data into a DOMComment basic test
--CREDITS--
Andrew Larssen <al@larssen.org>
London TestFest 2008
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$dom = new DomDocument();
$comment = $dom->createComment('test-comment');
$comment->replaceData(4,1,'replaced');
$dom->appendChild($comment);
echo $dom->saveXML();

// Replaces rest of string if count is greater than length of existing string
$dom = new DomDocument();
$comment = $dom->createComment('test-comment');
$comment->replaceData(0,50,'replaced');
$dom->appendChild($comment);
echo $dom->saveXML();

?>
--EXPECTF--
<?xml version="1.0"?>
<!--testreplacedcomment-->
<?xml version="1.0"?>
<!--replaced-->
