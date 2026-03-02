--TEST--
compareDocumentPosition: contains attribute as a direct descendent
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container align="center"/>
XML);

$container = $dom->documentElement;
$attribute = $container->attributes[0];

var_dump($container->compareDocumentPosition($attribute) === (DOMNode::DOCUMENT_POSITION_FOLLOWING | DOMNode::DOCUMENT_POSITION_CONTAINED_BY));
var_dump($attribute->compareDocumentPosition($container) === (DOMNode::DOCUMENT_POSITION_PRECEDING | DOMNode::DOCUMENT_POSITION_CONTAINS));

?>
--EXPECT--
bool(true)
bool(true)
