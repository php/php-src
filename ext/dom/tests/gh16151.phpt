--TEST--
GH-16151 (Assertion failure in ext/dom/parentnode/tree.c)
--EXTENSIONS--
dom
--FILE--
<?php

$element = new DOMElement("N", "W", "y");
$attr = new DOMAttr("c" , "n");
$doc = new DOMDocument();
$doc->appendChild($element);
$element->setAttributeNodeNS($attr);

try {
    $attr->insertBefore(new DOMComment("h"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $attr->appendChild(new DOMComment("h"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

$attr->insertBefore($doc->createEntityReference('amp'));
$attr->appendChild($doc->createEntityReference('amp'));

echo $doc->saveXML();

?>
--EXPECT--
Hierarchy Request Error
Hierarchy Request Error
<?xml version="1.0"?>
<N xmlns="y" c="n&amp;&amp;">W</N>
