--TEST--
Tests DOMNode::hasChildNodes()
--CREDITS--
Michael Stillwell <mjs@beebo.org>
# TestFest 2008
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$dom = new DOMDocument();

$dom->loadXML('<root/>');

echo $dom->saveXML();

echo "Document has child nodes\n";
var_dump($dom->documentElement->hasChildNodes());

echo "Document has child nodes\n";
$dom->loadXML('<root><a/></root>');
var_dump($dom->documentElement->hasChildNodes());

echo "Remove node and save\n";
$dom->documentElement->removeChild($dom->documentElement->firstChild);
echo $dom->saveXML();

echo "Document has child nodes\n";
var_dump($dom->documentElement->hasChildNodes());

echo "Document with 2 child nodes\n";
$dom->loadXML('<root><a/><b/></root>');
var_dump($dom->documentElement->hasChildNodes());

?>
--EXPECT--
<?xml version="1.0"?>
<root/>
Document has child nodes
bool(false)
Document has child nodes
bool(true)
Remove node and save
<?xml version="1.0"?>
<root/>
Document has child nodes
bool(false)
Document with 2 child nodes
bool(true)
