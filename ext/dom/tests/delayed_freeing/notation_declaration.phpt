--TEST--
Delayed freeing notation declaration
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<'XML'
<?xml version="1.0"?>
<!DOCTYPE books [
<!NOTATION myNotation SYSTEM "test.dtd">
]>
<container/>
XML);
$notation = $doc->doctype->notations[0];
var_dump($notation->nodeName, $notation->publicId, $notation->systemId);
$doc->removeChild($doc->doctype);
var_dump($notation->nodeName, $notation->publicId, $notation->systemId);
unset($doc);
var_dump($notation->nodeName, $notation->publicId, $notation->systemId);
?>
--EXPECT--
string(10) "myNotation"
string(0) ""
string(8) "test.dtd"
string(10) "myNotation"
string(0) ""
string(8) "test.dtd"
string(10) "myNotation"
string(0) ""
string(8) "test.dtd"
