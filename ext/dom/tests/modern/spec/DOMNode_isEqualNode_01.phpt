--TEST--
DOMNode::isEqualNode edge cases 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$dom->append($dom->createElement("container"));
$e1 = $dom->documentElement->appendChild($dom->createElementNS("urn:example1", "example:foo"));
$e1->after("\n");
$e2 = $dom->documentElement->appendChild($dom->createElementNS("urn:example2", "example:foo"));

$e2->setAttributeNS("urn:example2", "example2:bar", "baz");
$e2->setAttributeNS("urn:example1", "example2:bar", "baz");

$dom2 = DOM\HTMLDocument::createFromString("<!DOCTYPE HTML>" . $dom->saveHTML());

// Shouldn't match despite their string repesenation being the same
// because the namespaces are different
var_dump($dom->saveHTML($dom->getElementsByTagName("container")[0]));
var_dump($dom2->saveHTML($dom2->getElementsByTagName("container")[0]));
var_dump($dom->getElementsByTagName("container")[0]->isEqualNode($dom2->getElementsByTagName("container")[0]));

?>
--EXPECT--
string(115) "<container><example:foo></example:foo>
<example:foo example:bar="baz" example2:bar="baz"></example:foo></container>"
string(115) "<container><example:foo></example:foo>
<example:foo example:bar="baz" example2:bar="baz"></example:foo></container>"
bool(false)
