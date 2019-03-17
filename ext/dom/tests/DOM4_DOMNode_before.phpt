--TEST--
DOMNode::before()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark>first</mark></test>');
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$element = $dom->documentElement->firstChild;
print_node($element);
$element->before(
  $dom->createElement('inserted-before', 'content'),
  'text inserted before'
);
print_node($dom->documentElement->childNodes[0]);
print_node($dom->documentElement->childNodes[1]);
--EXPECT--
Node Name: mark
Node Type: 1
Num Children: 1
Node Content: first

Node Name: inserted-before
Node Type: 1
Num Children: 1
Node Content: content

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: text inserted before
