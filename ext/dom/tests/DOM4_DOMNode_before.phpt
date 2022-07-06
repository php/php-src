--TEST--
DOMNode::before()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark>first</mark><mark>second</mark></test>');

$element = $dom->documentElement->firstElementChild;
$secondMark = $dom->documentElement->lastElementChild;

$element->before(
  $dom->createElement('inserted-before', 'content'),
  'text inserted before'
);

$secondMark->before('text inserted before second');

print_node_list_compact($dom->documentElement->childNodes);
?>
--EXPECT--
<inserted-before>
  content
</inserted-before>
text inserted before
<mark>
  first
</mark>
text inserted before second
<mark>
  second
</mark>
