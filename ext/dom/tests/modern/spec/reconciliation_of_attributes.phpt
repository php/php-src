--TEST--
Reconciliation of attributes
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement('root'));

$root->setAttributeNS("urn:a", "a:root1", "bar");
$root1 = $root->getAttributeNodeNS("urn:a", "root1");
$root->setAttributeNS("urn:b", "a:root2", "bar");
$root2 = $root->getAttributeNodeNS("urn:b", "root2");

$child = $root->appendChild($dom->createElement("child"));
$child->setAttributeNS("urn:x", "a:child1", "bar");
$child1 = $child->getAttributeNodeNS("urn:x", "child1");

echo $dom->saveXML();

var_dump($root1->prefix, $root1->namespaceURI);
var_dump($root2->prefix, $root2->namespaceURI);

$child->removeAttribute("a:child1");
$root->setAttributeNodeNS($child1);

var_dump($child1->prefix, $child1->namespaceURI);

echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:b" xmlns:a="urn:a" a:root1="bar" a:root2="bar"><child xmlns:a="urn:x" a:child1="bar"/></root>
string(1) "a"
string(5) "urn:a"
string(1) "a"
string(5) "urn:b"
string(1) "a"
string(5) "urn:x"
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:x" xmlns:a="urn:b" xmlns:a="urn:a" a:root1="bar" a:root2="bar" a:child1="bar"><child xmlns:a="urn:x"/></root>
