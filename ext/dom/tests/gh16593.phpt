--TEST--
GH-16593 (Assertion failure in DOM->replaceChild)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$root = $doc->appendChild($doc->createElement('root'));
$child = $root->appendChild($doc->createElement('child'));
try {
    $root->replaceChild($doc->createAttribute('foo'), $child);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $doc->saveXML();

?>
--EXPECT--
Hierarchy Request Error
<?xml version="1.0"?>
<root><child/></root>
