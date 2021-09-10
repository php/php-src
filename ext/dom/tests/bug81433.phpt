--TEST--
Bug #81433 (DOMElement::setIdAttribute(attr, true) called twice removes ID)
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$dom = new DOMDocument('1.0', 'utf-8');

$element = $dom->createElement('test', 'root');

$dom->appendChild($element);

$element->setAttribute("id", 123);
$element->setIdAttribute("id", true);

$node = $element->getAttributeNode("id");
var_dump($node->isId());

$element->setIdAttribute("id", true);
var_dump($node->isId());
?>
--EXPECT--
bool(true)
bool(true)
