--TEST--
Element::hasAttribute()
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Get after parsing ---\n";

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><body align="foo" foo:bar="baz"></body></html>');
$body = $dom->getElementsByTagName("body")[0];
var_dump($body->hasAttribute("align"));
var_dump($body->hasAttribute("foo:bar"));

echo "--- Get after creating without namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttribute("prefix:local"));
$attr->value = "A";
var_dump($body->hasAttribute("prefix:local"));

echo "--- Get after creating with namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
var_dump($body->hasAttribute("prefix:local2"));
var_dump($body->hasAttribute("Prefix:LOCAL2"));

echo "--- Get after creating with namespace case sensitive ---\n";

$element = $dom->createElementNS("urn:a", "a:element");
$attr = $dom->createAttributeNS("urn:a", "Prefix:local2");
$element->setAttributeNode($attr);
$attr->value = "C";
var_dump($element->hasAttribute("Prefix:local2"));
var_dump($element->hasAttribute("Prefix:LOCAL2"));
var_dump($element->hasAttribute("prefix:local2"));

?>
--EXPECT--
--- Get after parsing ---
bool(true)
bool(true)
--- Get after creating without namespace ---
bool(true)
--- Get after creating with namespace ---
bool(true)
bool(true)
--- Get after creating with namespace case sensitive ---
bool(true)
bool(false)
bool(false)
