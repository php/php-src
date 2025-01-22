--TEST--
Serialize XML namespace on elements
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElementNS("http://www.w3.org/XML/1998/namespace", "xml:test"));
$root->appendChild($dom->createElementNS("http://www.w3.org/XML/1998/namespace", "xml:child"));

echo $dom->saveXml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<xml:test><xml:child/></xml:test>
