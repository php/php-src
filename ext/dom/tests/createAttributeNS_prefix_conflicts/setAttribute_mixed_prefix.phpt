--TEST--
DOMDocument::createAttributeNS() with prefix name conflict - setAttributeNode variation (DOM Level 3), mixed
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));

var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns1', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns1', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

?>
--EXPECT--
NULL
string(18) "http://php.net/ns1"
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" foo:hello=""/>
