--TEST--
DOMDocument::adoptNode() with attribute references
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$root = $dom->appendChild($dom->createElement('root'));
$root->setAttributeNS("urn:a", "a:root1", "bar");
$root1 = $root->getAttributeNodeNS("urn:a", "root1");
echo $dom->saveXML();

$dom = new DOMDocument;
$dom->appendChild($dom->adoptNode($root));
foreach ($dom->documentElement->attributes as $attr) {
    var_dump($attr->namespaceURI, $attr->prefix, $attr->localName, $attr->nodeValue);
}

?>
--EXPECT--
<?xml version="1.0"?>
<root xmlns:a="urn:a" a:root1="bar"/>
string(5) "urn:a"
string(1) "a"
string(5) "root1"
string(3) "bar"
