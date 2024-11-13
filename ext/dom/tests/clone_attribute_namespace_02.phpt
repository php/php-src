--TEST--
Cloning an attribute should retain its namespace 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container xmlns:foo="some:ns" foo:bar="1">
    <child xmlns:foo="some:other"/>
</container>
XML);

$clone = $dom->documentElement->getAttributeNodeNs("some:ns", "bar")->cloneNode(true);
$dom->documentElement->firstElementChild->setAttributeNodeNs($clone);

echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns:foo="some:ns" foo:bar="1">
    <child xmlns:foo="some:other" xmlns:foo1="some:ns" foo1:bar="1"/>
</container>
