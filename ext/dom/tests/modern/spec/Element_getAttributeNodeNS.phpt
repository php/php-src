--TEST--
Element::getAttributeNodeNS()
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Get after parsing ---\n";

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><body align="foo" foo:bar="baz"></body></html>');
$body = $dom->getElementsByTagName("body")[0];

echo "--- After parsing, i.e. without namespace ---\n";

// Every pair of 2 calls should return the same result
var_dump($body->getAttributeNodeNS(NULL, "align")->textContent);
var_dump($body->getAttributeNodeNS("", "align")->textContent);
var_dump($body->getAttributeNodeNS(NULL, "ALIGN"));
var_dump($body->getAttributeNodeNS("", "ALIGN"));
var_dump($body->getAttributeNodeNS(NULL, "foo:bar")->textContent);
var_dump($body->getAttributeNodeNS("", "foo:bar")->textContent);
var_dump($body->getAttributeNodeNS(NULL, "FOO:BAR"));
var_dump($body->getAttributeNodeNS("", "FOO:BAR"));

echo "--- Special legacy case ---\n";

var_dump($dom->documentElement->getAttributeNodeNS("http://www.w3.org/2000/xmlns/", "xmlns"));

echo "--- Get after creating without namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttribute("prefix:local"));
$attr->value = "A";
var_dump($body->getAttributeNodeNS(NULL, "prefix:local")->textContent);
var_dump($body->getAttributeNodeNS(NULL, "prefix:LOCAL"));

echo "--- Get after creating with namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
var_dump($body->getAttributeNodeNS("urn:a", "local2")->textContent);
var_dump($body->getAttributeNodeNS("urn:a", "LOCAL2"));

?>
--EXPECT--
--- Get after parsing ---
--- After parsing, i.e. without namespace ---
string(3) "foo"
string(3) "foo"
NULL
NULL
string(3) "baz"
string(3) "baz"
NULL
NULL
--- Special legacy case ---
NULL
--- Get after creating without namespace ---
string(1) "A"
NULL
--- Get after creating with namespace ---
string(1) "B"
NULL
