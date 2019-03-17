--TEST--
DOMNode::after()
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
$element->after(
  'text inserted after',
  $dom->createElement('inserted-after', 'content')
);
print_node($dom->documentElement->childNodes[1]);
print_node($dom->documentElement->childNodes[2]);
--EXPECT--
Node Name: mark
Node Type: 1
Num Children: 1
Node Content: first

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: text inserted after

Node Name: inserted-after
Node Type: 1
Num Children: 1
Node Content: content
