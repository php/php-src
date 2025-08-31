--TEST--
compareDocumentPosition: contains attribute as a descendent in a longer path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <div>
        <p align="center"/>
    </div>
</container>
XML);

$container = $dom->documentElement;
$p = $container->firstElementChild->firstElementChild;
$attribute = $p->attributes[0];

var_dump($container->compareDocumentPosition($attribute) === (DOMNode::DOCUMENT_POSITION_FOLLOWING | DOMNode::DOCUMENT_POSITION_CONTAINED_BY));
var_dump($attribute->compareDocumentPosition($container) === (DOMNode::DOCUMENT_POSITION_PRECEDING | DOMNode::DOCUMENT_POSITION_CONTAINS));

?>
--EXPECT--
bool(true)
bool(true)
