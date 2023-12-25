--TEST--
Default namespace move
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElementNS('urn:a', 'foo:bar'));
$child = $root->appendChild($dom->createElementNS('urn:a', 'foo:baz'));
$child->setAttributeNS("urn:b", "foo:attr", "value");
$root->remove();
$dom->appendChild($child);
echo $dom->saveXML(), "\n";

$dom2 = DOM\XMLDocument::createEmpty();
$child = $dom2->importNode($child, true);
$dom2->appendChild($child);
echo $dom2->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<foo:baz xmlns:foo="urn:a" xmlns:ns1="urn:b" ns1:attr="value"/>
<?xml version="1.0" encoding="UTF-8"?>
<foo:baz xmlns:foo="urn:a" xmlns:ns1="urn:b" ns1:attr="value"/>
