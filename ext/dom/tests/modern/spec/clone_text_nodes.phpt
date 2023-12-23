--TEST--
Cloning text nodes should not merge adjacent text nodes
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElementNS("urn:a", "a:root"));
$root->setAttribute("foo", "bar");

$root->appendChild($dom->createTextNode("a"));
$root->appendChild($dom->createTextNode("foo"));
$root->appendChild($dom->createTextNode("b"));
$child = $root->appendChild($dom->createElement("child"));
$child->appendChild($dom->createTextNode("c"));
$child2 = $root->appendChild($dom->createElement("child2"));
$child2->appendChild($dom->createTextNode("d"));

echo $dom->saveXML();

$clone = clone $root;
var_dump($clone->firstChild->textContent);
var_dump($clone->firstChild->nextSibling->textContent);
var_dump($clone->firstChild->nextSibling->nextSibling->textContent);
echo $dom->saveXML($clone), "\n";

$clone = $child2->cloneNode(true);
echo $dom->saveXML($clone), "\n";

$clone = $child2->cloneNode(false);
echo $dom->saveXML($clone), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<a:root xmlns:a="urn:a" foo="bar">afoob<child>c</child><child2>d</child2></a:root>
string(1) "a"
string(3) "foo"
string(1) "b"
<a:root xmlns:a="urn:a" foo="bar">afoob<child>c</child><child2>d</child2></a:root>
<child2>d</child2>
<child2/>
