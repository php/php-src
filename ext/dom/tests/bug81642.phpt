--TEST--
Bug #81642 DOMChildNode::replaceWith() bug when replacing a node with itself.
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark>first</mark><mark>second</mark></test>');
$element = $dom->documentElement->firstChild;
$element->replaceWith($element);

print_node_list_compact($dom->documentElement->childNodes);
?>
--EXPECT--
<test><mark>first</mark><mark>second</mark></test>

