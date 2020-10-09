--TEST--
DOMParentNode: Child Element Handling
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test></test>');

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createTextNode('foo'));
$fragment->appendChild($dom->createElement('bar', 'FirstElement'));
$fragment->appendChild($dom->createElement('bar', 'LastElement'));
$fragment->appendChild($dom->createTextNode('bar'));

var_dump($fragment instanceof DOMParentNode);
print_node($fragment->firstElementChild);
print_node($fragment->lastElementChild);
var_dump($fragment->childElementCount);
var_dump($fragment->lastElementChild->firstElementChild);
var_dump($fragment->lastElementChild->lastElementChild);
var_dump($fragment->lastElementChild->childElementCount);
?>
--EXPECT--
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
