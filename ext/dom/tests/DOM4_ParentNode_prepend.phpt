--TEST--
DOMParentNode::prepend()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark/></test>');
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$element = $dom->documentElement;
$element->prepend(
  $dom->createElement('element', 'content'),
  'content'
);

print_node($element->childNodes[0]);
print_node($element->childNodes[1]);
print_node($element->childNodes[2]);
--EXPECT--
Node Name: element
Node Type: 1
Num Children: 1
Node Content: content

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: content

Node Name: mark
Node Type: 1
Num Children: 0
Node Content:
