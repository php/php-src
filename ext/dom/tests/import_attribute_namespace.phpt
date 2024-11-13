--TEST--
Cloning an attribute should retain its namespace 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container xmlns:foo="some:ns" foo:bar="1"/>
XML);

$dom2 = new DOMDocument;
$dom2->loadXML(<<<XML
<?xml version="1.0"?>
<container xmlns:foo="some:other"/>
XML);

$imported = $dom2->importNode($dom->documentElement->getAttributeNodeNs("some:ns", "bar"));
var_dump($imported->prefix, $imported->namespaceURI);
$dom2->documentElement->setAttributeNodeNs($imported);
var_dump($imported->prefix, $imported->namespaceURI);

echo $dom->saveXML();
echo $dom2->saveXML();

?>
--EXPECT--
string(7) "default"
string(7) "some:ns"
string(7) "default"
string(7) "some:ns"
<?xml version="1.0"?>
<container xmlns:foo="some:ns" foo:bar="1"/>
<?xml version="1.0"?>
<container xmlns:foo="some:other" xmlns:default="some:ns" default:bar="1"/>
