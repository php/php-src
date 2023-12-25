--TEST--
GH-11404 (DOMDocument::savexml and friends ommit xmlns="" declaration for null namespace, creating incorrect xml representation of the DOM)
--EXTENSIONS--
dom
--FILE--
<?php

$dom1 = DOM\XMLDocument::createFromString('<?xml version="1.0"?><with xmlns="some:ns" />');

$nodeA = $dom1->createElement('none');
$nodeB = $dom1->createElementNS(null, 'none');

$dom1->documentElement->appendChild($nodeA);
$dom1->documentElement->appendChild($nodeB);

echo $dom1->saveXML(), "\n";

var_dump($nodeA->namespaceURI, $nodeB->namespaceURI);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<with xmlns="some:ns"><none xmlns=""/><none xmlns=""/></with>
NULL
NULL
