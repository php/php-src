--TEST--
DOMParentNode: Child Element Handling
--EXTENSIONS--
dom
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test>foo<bar>FirstElement</bar><bar>LastElement</bar>bar</test>');

var_dump($dom instanceof DOMParentNode);
print_node($dom->firstElementChild);
print_node($dom->lastElementChild);
var_dump($dom->childElementCount);

$element = $dom->documentElement;
var_dump($element instanceof DOMParentNode);
print_node($element->firstElementChild);
print_node($element->lastElementChild);
var_dump($element->childElementCount);
var_dump($element->lastElementChild->firstElementChild);
var_dump($element->lastElementChild->lastElementChild);
var_dump($element->lastElementChild->childElementCount);
?>
--EXPECT--
bool(true)
Node Name: test
Node Type: 1
Num Children: 4

Node Name: test
Node Type: 1
Num Children: 4

int(1)
bool(true)
Node Name: bar
Node Type: 1
Num Children: 1
Node Content: FirstElement

Node Name: bar
Node Type: 1
Num Children: 1
Node Content: LastElement

int(2)
NULL
NULL
int(0)
