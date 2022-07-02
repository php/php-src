--TEST--
DOMDocument::createEntityReference() should create a new entity reference node
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument('1.0');
$ref = $dom->createEntityReference('nbsp');
$dom->appendChild($ref);
echo $dom->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
&nbsp;
