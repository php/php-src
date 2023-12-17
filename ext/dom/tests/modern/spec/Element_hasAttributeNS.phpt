--TEST--
Element::hasAttributeNS()
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Get after parsing ---\n";

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><body align="foo" foo:bar="baz"></body></html>');
$body = $dom->getElementsByTagName("body")[0];

echo "--- After parsing, i.e. without namespace ---\n";

// Every pair of 2 calls should return the same result
var_dump($body->hasAttributeNS(NULL, "align"));
var_dump($body->hasAttributeNS("", "align"));
var_dump($body->hasAttributeNS(NULL, "ALIGN"));
var_dump($body->hasAttributeNS("", "ALIGN"));
var_dump($body->hasAttributeNS(NULL, "foo:bar"));
var_dump($body->hasAttributeNS("", "foo:bar"));
var_dump($body->hasAttributeNS(NULL, "FOO:BAR"));
var_dump($body->hasAttributeNS("", "FOO:BAR"));

echo "--- Special legacy case ---\n";

var_dump($dom->documentElement->hasAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns"));

echo "--- Get after creating without namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttribute("prefix:local"));
$attr->value = "A";
var_dump($body->hasAttributeNS(NULL, "prefix:local"));
var_dump($body->hasAttributeNS(NULL, "prefix:LOCAL"));

echo "--- Get after creating with namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
var_dump($body->hasAttributeNS("urn:a", "local2"));
var_dump($body->hasAttributeNS("urn:a", "LOCAL2"));

?>
--EXPECT--
--- Get after parsing ---
--- After parsing, i.e. without namespace ---
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
--- Special legacy case ---
bool(false)
--- Get after creating without namespace ---
bool(true)
bool(false)
--- Get after creating with namespace ---
bool(true)
bool(false)
