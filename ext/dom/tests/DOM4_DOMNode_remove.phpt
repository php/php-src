--TEST--
DOMNode::remove()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><one>first</one><two>second</two></test>');
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$element = $dom->documentElement;
print_node($element->firstChild);
$returnValue = $element->firstChild->remove();
print_node($element->firstChild);
var_dump($returnValue);
?>
--EXPECT--
Node Name: one
Node Type: 1
Num Children: 1
Node Content: first

Node Name: two
Node Type: 1
Num Children: 1
Node Content: second

NULL

