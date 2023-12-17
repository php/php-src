--TEST--
Element::getAttribute()
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Get after parsing ---\n";

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><body align="foo" foo:bar="baz"></body></html>');
$body = $dom->getElementsByTagName("body")[0];
var_dump($body->getAttribute("align"));
var_dump($body->getAttribute("foo:bar"));

echo "--- Get after creating without namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttribute("prefix:local"));
$attr->value = "A";
var_dump($body->getAttribute("prefix:local"));

echo "--- Get after creating with namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
var_dump($body->getAttribute("prefix:local2"));
var_dump($body->getAttribute("Prefix:LOCAL2"));

echo "--- Get after creating with namespace case sensitive ---\n";

$element = $dom->createElementNS("urn:a", "a:element");
$attr = $dom->createAttributeNS("urn:a", "Prefix:local2");
$element->setAttributeNode($attr);
$attr->value = "C";
var_dump($element->getAttribute("Prefix:local2"));
var_dump($element->getAttribute("Prefix:LOCAL2"));
var_dump($element->getAttribute("prefix:local2"));

?>
--EXPECT--
--- Get after parsing ---
string(3) "foo"
string(3) "baz"
--- Get after creating without namespace ---
string(1) "A"
--- Get after creating with namespace ---
string(1) "B"
string(1) "B"
--- Get after creating with namespace case sensitive ---
string(1) "C"
string(0) ""
string(0) ""
