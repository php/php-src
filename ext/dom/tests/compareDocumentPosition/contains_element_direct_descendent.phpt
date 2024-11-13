--TEST--
compareDocumentPosition: contains nodes as a direct descendent
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
<div/>
</container>
XML);

$container = $dom->documentElement;
$div = $container->firstChild;

var_dump($container->compareDocumentPosition($div) === (DOMNode::DOCUMENT_POSITION_FOLLOWING | DOMNode::DOCUMENT_POSITION_CONTAINED_BY));
var_dump($div->compareDocumentPosition($container) === (DOMNode::DOCUMENT_POSITION_PRECEDING | DOMNode::DOCUMENT_POSITION_CONTAINS));

?>
--EXPECT--
bool(true)
bool(true)
