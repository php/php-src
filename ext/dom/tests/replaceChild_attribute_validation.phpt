--TEST--
replaceChild with attribute children
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$attr = $dom->createAttribute('attr');
$attr->textContent = "test";

try {
    $attr->replaceChild($dom->createProcessingInstruction('pi'), $attr->firstChild);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

$root = $dom->appendChild($dom->createElement('root'));
$root->setAttributeNode($attr);

echo $dom->saveXML();

?>
--EXPECT--
Hierarchy Request Error
<?xml version="1.0"?>
<root attr="test"/>
