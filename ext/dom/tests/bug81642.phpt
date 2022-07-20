--TEST--
Bug #81642 DOMChildNode::replaceWith() bug when replacing a node with itself.
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$doc = new DOMDocument;
$headNode = $doc->createElement("head");
$doc->appendChild($headNode);
$titleNode = $doc->createElement("title");
$headNode->appendChild($titleNode);
$titleNode->replaceWith($titleNode);
echo $doc->saveXML().PHP_EOL;

$doc = new DOMDocument;
$headNode = $doc->createElement("head");
$doc->appendChild($headNode);
$titleNode = $doc->createElement("title");
$headNode->appendChild($titleNode);
$titleNode->replaceWith($titleNode, 'foo');
echo $doc->saveXML().PHP_EOL;

?>
--EXPECT--
<?xml version="1.0"?>
<head><title/></head>

<?xml version="1.0"?>
<head><title/>foo</head>

