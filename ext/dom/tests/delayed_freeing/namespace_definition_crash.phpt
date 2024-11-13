--TEST--
Delayed freeing namespace definition should not crash
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->appendChild($doc->createElement('container'));
$child = $doc->documentElement->appendChild($doc->createElementNS('some:ns', 'child'));
$child_child = $child->appendChild($doc->createElementNS('some:ns', 'x'));

echo $doc->saveXML();

$child->remove();
echo $doc->saveXML();

unset($child);
var_dump($child_child->namespaceURI);
?>
--EXPECT--
<?xml version="1.0"?>
<container><child xmlns="some:ns"><x/></child></container>
<?xml version="1.0"?>
<container/>
string(7) "some:ns"
