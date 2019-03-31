--TEST--
DOMParentNode::append() adopts node from other document
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$source = new DOMDocument;
$source->loadXML('<test><foo>content</foo></test>');
if(!$source) {
  echo "Error while parsing the document\n";
  exit;
}
$target = new DOMDocument;
$target->loadXML('<test><mark>first</mark></test>');
if(!$target) {
  echo "Error while parsing the document\n";
  exit;
}

$element = $source->documentElement->firstChild;
$target->documentElement->append($element);

if($element->ownerDocument !== $target || $element !== $target->documentElement->childNodes[1]) {
  echo "Node was not adopted into target document\n";
  exit;
}

print_node($target->documentElement->childNodes[0]);
print_node($target->documentElement->childNodes[1]);
print_node($source->documentElement);

--EXPECT--
Node Name: mark
Node Type: 1
Num Children: 1
Node Content: first

Node Name: foo
Node Type: 1
Num Children: 1
Node Content: content

Node Name: test
Node Type: 1
Num Children: 0
Node Content:
