--TEST--
DOMNode::remove()
--EXTENSIONS--
dom
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><one>first</one><two>second</two></test>');

$element = $dom->documentElement;
print_node($element->firstChild);
$returnValue = $element->firstChild->remove();
print_node($element->firstChild);
var_dump($returnValue);
?>
--EXPECT--
Node Name: one
Node Type: 1
Num Children: 1
Node Content: first

Node Name: two
Node Type: 1
Num Children: 1
Node Content: second

NULL
