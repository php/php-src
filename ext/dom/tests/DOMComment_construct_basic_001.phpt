--TEST--
DOMComment::__construct() with constructor called twice.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-25
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument('1.0', 'UTF-8');
$element = $dom->appendChild(new DOMElement('root'));
$comment = new DOMComment("This is the first comment.");
$comment->__construct("This is the second comment.");
$comment = $element->appendChild($comment);
print $dom->saveXML();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root><!--This is the second comment.--></root>
