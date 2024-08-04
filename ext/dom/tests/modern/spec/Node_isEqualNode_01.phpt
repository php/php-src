--TEST--
Dom\Node::isEqualNode edge cases 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$dom->append($dom->createElement("container"));
$e1 = $dom->documentElement->appendChild($dom->createElementNS("urn:example1", "example:foo"));
$e1->after("\n");
$e2 = $dom->documentElement->appendChild($dom->createElementNS("urn:example2", "example:foo"));

$e2->setAttributeNS("urn:example2", "example2:bar", "baz1");
$e2->setAttributeNS("urn:example1", "example2:bar", "baz2");

$dom2 = Dom\HTMLDocument::createFromString("<!DOCTYPE HTML>" . $dom->saveHtml());

// Shouldn't match because the attributes can't roundtrip.
var_dump($dom->saveHtml($dom->getElementsByTagName("container")[0]));
var_dump($dom2->saveHtml($dom2->getElementsByTagName("container")[0]));
var_dump($dom->getElementsByTagName("container")[0]->isEqualNode($dom2->getElementsByTagName("container")[0]));

?>
--EXPECT--
string(118) "<container><example:foo></example:foo>
<example:foo example2:bar="baz1" example2:bar="baz2"></example:foo></container>"
string(98) "<container><example:foo></example:foo>
<example:foo example2:bar="baz1"></example:foo></container>"
bool(false)
