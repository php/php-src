--TEST--
DOMParentNode::append() with attributes
--EXTENSIONS--
dom
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test attr-one="21"/>');

$replacement = $dom->createAttribute('attr-one');
$replacement->value ='42';
$addition = $dom->createAttribute('attr-two');
$addition->value = '23';

$element = $dom->documentElement;

try {
    $element->append($replacement, $addition);
} catch (DOMException $e) {
    echo $e->getMessage() . "\n";
}

echo $dom->saveXML();
?>
--EXPECT--
Hierarchy Request Error
<?xml version="1.0"?>
<test attr-one="21"/>
