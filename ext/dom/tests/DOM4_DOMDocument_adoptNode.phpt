--TEST--
DOMDocumentNode::adoptNode()
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
$adoptedElement = $target->adoptNode($element);

if($adoptedElement !== $element) {
  echo "Adopted element must be the same as element passed to adoptNode()\n";
  exit;
}
if($element->ownerDocument !== $target) {
  echo "Node was not adopted into target document\n";
  exit;
}

$target->documentElement->appendChild($adoptedElement);

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
