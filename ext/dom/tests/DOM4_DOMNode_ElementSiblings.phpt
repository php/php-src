--TEST--
DOMNode: Element Siblings
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test>foo<bar>FirstElement</bar><bar>LastElement</bar>bar</test>');

$element = $dom->documentElement;
print_node($element->firstElementChild->nextElementSibling);
print_node($element->lastElementChild->previousElementSibling);
?>
--EXPECT--
Node Name: bar
Node Type: 1
Num Children: 1
Node Content: LastElement

Node Name: bar
Node Type: 1
Num Children: 1
Node Content: FirstElement
