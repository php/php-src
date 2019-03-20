--TEST--
DOMParentNode::append()
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
$element->append(
  $dom->createElement('element', 'content'),
  'content'
);

var_dump($dom->documentElement->childElementCount);
print_node($element->childNodes[0]);
print_node($element->childNodes[1]);
print_node($element->childNodes[2]);
--EXPECT--
int(2)
Node Name: mark
Node Type: 1
Num Children: 0
Node Content: 

Node Name: element
Node Type: 1
Num Children: 1
Node Content: content

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: content
