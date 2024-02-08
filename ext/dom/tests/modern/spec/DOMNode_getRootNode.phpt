--TEST--
DOMNode::getRootNode()
--EXTENSIONS--
dom
--FILE--
<?php

// Same as DOMNode_getRootNode.phpt but for modern DOM

$dom = DOM\XMLDocument::createFromString('<?xml version="1.0"?><html><body/></html>');

var_dump($dom->documentElement->firstElementChild->getRootNode() === $dom);
$p = $dom->createElement('p');
var_dump($p->getRootNode() === $p);
$dom->documentElement->appendChild($p);
var_dump($p->getRootNode() === $dom);
$dom->documentElement->remove();
var_dump($p->getRootNode() === $p);

$fragment = $dom->createDocumentFragment();
var_dump($fragment->getRootNode() === $fragment);
$div = $fragment->appendChild($dom->createElement('div'));
$div->appendChild($p);
var_dump($p->getRootNode() === $fragment);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
