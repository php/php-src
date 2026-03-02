--TEST--
DOMNode::normalize() advanced
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$root = $dom->appendChild($dom->createElement("root"));
$child1 = $root->appendChild($dom->createElement("child1"));
$child2 = $root->appendChild($dom->createElement("child2"));

$child1->appendChild($dom->createTextNode("abc"));
$child1->appendChild($dom->createTextNode(""));
$child1->appendChild($dom->createTextNode("def"));

$child2->appendChild($dom->createTextNode(""));
$child2->setAttribute("foo", "bar");

echo $dom->saveXML();

$root->normalize();

var_dump($child1->childNodes[0]->textContent);
var_dump($child2->childNodes[0]);

?>
--EXPECT--
<?xml version="1.0"?>
<root><child1>abcdef</child1><child2 foo="bar"></child2></root>
string(6) "abcdef"
NULL
