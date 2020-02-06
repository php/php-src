--TEST--
DOMNode::replaceWith()
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
print_node($dom->documentElement);
$element->replaceWith(
  $dom->createElement('element', 'content'),
  'content'
);

print_node($dom->documentElement);
print_node($dom->documentElement->childNodes[0]);
print_node($dom->documentElement->childNodes[1]);
?>
--EXPECT--
Node Name: test
Node Type: 1
Num Children: 1
Node Content: first

Node Name: test
Node Type: 1
Num Children: 2

Node Name: element
Node Type: 1
Num Children: 1
Node Content: content

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: content
