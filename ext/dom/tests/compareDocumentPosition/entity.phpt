--TEST--
compareDocumentPosition: entity ordering
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<!DOCTYPE root [
<!ENTITY e1 "e1">
<!ENTITY e2 "e2">
]>
<container>
    <child>&e1;</child>
</container>
XML, LIBXML_NOENT);

$entities = iterator_to_array($dom->doctype->entities);
usort($entities, fn ($a, $b) => strcmp($a->nodeName, $b->nodeName));

echo "--- Compare entities ---\n";

var_dump($entities[0]->compareDocumentPosition($entities[1]) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
var_dump($entities[1]->compareDocumentPosition($entities[0]) === DOMNode::DOCUMENT_POSITION_PRECEDING);

$xpath = new DOMXPath($dom);
$child = $xpath->query('//child')->item(0);

echo "--- Compare entities against first child ---\n";

var_dump($entities[0]->compareDocumentPosition($child->firstChild) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
var_dump($entities[1]->compareDocumentPosition($child->firstChild) === DOMNode::DOCUMENT_POSITION_FOLLOWING);

echo "--- Compare first child against entities ---\n";

var_dump($child->firstChild->compareDocumentPosition($entities[0]) === DOMNode::DOCUMENT_POSITION_PRECEDING);
var_dump($child->firstChild->compareDocumentPosition($entities[1]) === DOMNode::DOCUMENT_POSITION_PRECEDING);

?>
--EXPECT--
--- Compare entities ---
bool(true)
bool(true)
--- Compare entities against first child ---
bool(true)
bool(true)
--- Compare first child against entities ---
bool(true)
bool(true)
