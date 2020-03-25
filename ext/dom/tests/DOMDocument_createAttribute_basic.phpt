--TEST--
DomDocument::createAttribute() - basic test for DomDocument::createAttribute()
--CREDITS--
Muhammad Khalid Adnan
# TestFest 2008
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$node = $doc->createElement("para");
$newnode = $doc->appendChild($node);

// A pass case.
$test_attribute = $doc->createAttribute("hahaha");
$node->appendChild($test_attribute);

echo $doc->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<para hahaha=""/>
