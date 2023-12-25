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
$root->setAttributeNS("urn:a", "a:root3", "bar");
$root3 = $root->getAttributeNodeNS("urn:a", "root3");

$child = $root->appendChild($dom->createElement("child"));
$child->setAttributeNS("urn:x", "a:child1", "bar");
$child1 = $child->getAttributeNodeNS("urn:x", "child1");
$child->setAttributeNS("urn:a", "a:child2", "bar");
$child2 = $child->getAttributeNodeNS("urn:a", "child2");

echo $dom->saveXML(), "\n";

var_dump($root1->prefix, $root1->namespaceURI);
var_dump($root2->prefix, $root2->namespaceURI);
var_dump($root3->prefix, $root3->namespaceURI);

$child->removeAttribute("a:child1");
$root->setAttributeNodeNS($child1);

var_dump($child1->prefix, $child1->namespaceURI);
var_dump($child2->prefix, $child2->namespaceURI);

echo $dom->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:a" a:root1="bar" xmlns:ns1="urn:b" ns1:root2="bar" a:root3="bar"><child xmlns:a="urn:x" a:child1="bar" xmlns:ns2="urn:a" ns2:child2="bar"/></root>
string(1) "a"
string(5) "urn:a"
string(1) "a"
string(5) "urn:b"
string(1) "a"
string(5) "urn:a"
string(1) "a"
string(5) "urn:x"
string(1) "a"
string(5) "urn:a"
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:a" a:root1="bar" xmlns:ns1="urn:b" ns1:root2="bar" a:root3="bar" xmlns:ns2="urn:x" ns2:child1="bar"><child a:child2="bar"/></root>
