--TEST--
DOMNode::replaceWith()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark>first</mark><mark>second</mark></test>');

$element = $dom->documentElement->firstChild;
$element->replaceWith(
  $dom->createElement('element', 'content'),
  'content'
);

print_node_list_compact($dom->documentElement->childNodes);
?>
--EXPECT--
<element>
  content
</element>
content
<mark>
  second
</mark>
