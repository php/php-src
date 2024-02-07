--TEST--
Element::removeAttribute()
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Remove after parsing ---\n";

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><body align="foo" foo:bar="baz"></body></html>');
$body = $dom->getElementsByTagName("body")[0];
var_dump($body->removeAttribute("align"));
var_dump($body->removeAttribute("foo:bar"));
echo $dom->saveHTML($body), "\n";

echo "--- Remove after creating without namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttribute("prefix:local"));
$attr->value = "A";
var_dump($body->removeAttribute("prefix:local"));
echo $dom->saveHTML($body), "\n";

echo "--- Remove after creating with namespace ---\n";

$body->setAttributeNode($attr = $dom->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
var_dump($body->removeAttribute("prefix:local2"));
echo $dom->saveHTML($body), "\n";
$body->setAttributeNode($attr = $dom->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
var_dump($body->removeAttribute("Prefix:LOCAL2"));
echo $dom->saveHTML($body), "\n";

echo "--- Remove after creating with namespace case sensitive ---\n";

$element = $dom->createElementNS("urn:a", "a:element");
$attr = $dom->createAttributeNS("urn:a", "Prefix:local2");
$element->setAttributeNode($attr);
$attr->value = "C";
var_dump($element->removeAttribute("Prefix:local2"));
var_dump($element->removeAttribute("Prefix:LOCAL2"));
var_dump($element->removeAttribute("prefix:local2"));
echo $dom->saveHTML($body), "\n";

?>
--EXPECT--
--- Remove after parsing ---
NULL
NULL
<body></body>
--- Remove after creating without namespace ---
NULL
<body></body>
--- Remove after creating with namespace ---
NULL
<body></body>
NULL
<body></body>
--- Remove after creating with namespace case sensitive ---
NULL
NULL
NULL
<body></body>
