--TEST--
DOMDocumentFragment::appendXML() with children with properties.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$fragment = $document->createDocumentFragment();
$fragment->appendXML('<foo id="baz">bar</foo>');
$root->appendChild($fragment);

print $document->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<root><foo id="baz">bar</foo></root>
