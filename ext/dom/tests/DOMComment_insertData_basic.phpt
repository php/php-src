--TEST--
Test inserting data into a DOMComment basic test
--CREDITS--
Andrew Larssen <al@larssen.org>
London TestFest 2008
--EXTENSIONS--
dom
--FILE--
<?php

//correct offset
$dom = new DomDocument();
$comment = $dom->createComment('test-comment');
$comment->insertData(4,'-inserted');
$dom->appendChild($comment);
echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<!--test-inserted-comment-->
