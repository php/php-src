--TEST--
Element::getAttributeNS()
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Get after parsing ---\n";

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><body align="foo" foo:bar="baz"></body></html>');
$body = $dom->getElementsByTagName("body")[0];

echo "--- After parsing, i.e. without namespace ---\n";

// Every pair of 2 calls should return the same result
var_dump($body->getAttributeNS(NULL, "align"));
var_dump($body->getAttributeNS("", "align"));
var_dump($body->getAttributeNS(NULL, "ALIGN"));
var_dump($body->getAttributeNS("", "ALIGN"));
var_dump($body->getAttributeNS(NULL, "foo:bar"));
var_dump($body->getAttributeNS("", "foo:bar"));
var_dump($body->getAttributeNS(NULL, "FOO:BAR"));
var_dump($body->getAttributeNS("", "FOO:BAR"));

echo "--- Special legacy case ---\n";

var_dump($dom->documentElement->getAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns"));

echo "--- Get after creating without namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttribute("prefix:local"));
$attr->value = "A";
var_dump($body->getAttributeNS(NULL, "prefix:local"));
var_dump($body->getAttributeNS(NULL, "prefix:LOCAL"));

echo "--- Get after creating with namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
var_dump($body->getAttributeNS("urn:a", "local2"));
var_dump($body->getAttributeNS("urn:a", "LOCAL2"));

?>
--EXPECT--
--- Get after parsing ---
--- After parsing, i.e. without namespace ---
string(3) "foo"
string(3) "foo"
string(0) ""
string(0) ""
string(3) "baz"
string(3) "baz"
string(0) ""
string(0) ""
--- Special legacy case ---
string(0) ""
--- Get after creating without namespace ---
string(1) "A"
string(0) ""
--- Get after creating with namespace ---
string(1) "B"
string(0) ""
