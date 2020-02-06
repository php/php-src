--TEST--
DOMParentNode::prepend()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark/><mark><nested /></mark><mark/></test>');
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$element = $dom->documentElement;
var_dump($element->childElementCount);

$element->prepend(
  $dom->createElement('element', 'content'),
  'content'
);

print_node($element->childNodes[0]);
print_node($element->childNodes[1]);
print_node($element->childNodes[2]);
print_node($element->childNodes[3]);
print_node($element->childNodes[4]);

$element = $dom->documentElement;
$element->prepend(
  $dom->createElement('element', 'content'),
  'content'
);
var_dump($element->childElementCount);
--EXPECT--
int(3)
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

Node Name: mark
Node Type: 1
Num Children: 1
Node Content:

Node Name: mark
Node Type: 1
Num Children: 0
Node Content:

int(5)
