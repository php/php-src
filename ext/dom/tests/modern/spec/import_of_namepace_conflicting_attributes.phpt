--TEST--
Import of namespace-conflicting attributes
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

$importer = DOM\XMLDocument::createEmpty();
$imported = $importer->importNode($root, true);
echo $importer->saveXML($imported), "\n";

var_dump($imported->getAttributeNodeNS("urn:a", "root1")->prefix);
var_dump($imported->getAttributeNodeNS("urn:a", "root1")->namespaceURI);
var_dump($imported->getAttributeNodeNS("urn:b", "root2")->prefix);
var_dump($imported->getAttributeNodeNS("urn:b", "root2")->namespaceURI);
var_dump($imported->getAttributeNodeNS("urn:x", "child1")->prefix);
var_dump($imported->getAttributeNodeNS("urn:x", "child1")->namespaceURI);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:a="urn:b" xmlns:a="urn:a" a:root1="bar" a:root2="bar"><child xmlns:a="urn:x" a:child1="bar"/></root>
string(1) "a"
string(5) "urn:a"
string(1) "a"
string(5) "urn:b"
<root xmlns:a="urn:x" xmlns:a="urn:b" xmlns:a="urn:a" a:root1="bar" a:root2="bar" a:child1="bar"><child/></root>
string(1) "a"
string(5) "urn:a"
string(1) "a"
string(5) "urn:b"
string(1) "a"
string(5) "urn:x"
