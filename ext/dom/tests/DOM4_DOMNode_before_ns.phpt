--TEST--
DOMNode::before() with namespace
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$doc  = new DOMDocument('1.0', 'utf-8');
$doc->formatOutput = true;

$root = $doc->createElementNS('http://www.w3.org/2005/Atom', 'element');
$doc->appendChild($root);
$root->setAttributeNS('http://www.w3.org/2000/xmlns/' ,'xmlns:g', 'http://base.google.com/ns/1.0');

$item = $doc->createElementNS('http://base.google.com/ns/1.0', 'g:item_type', 'house');
$root->append($item);

$item2 = $doc->createElementNS('http://base.google.com/ns/1.0', 'g:item_type', 'street');
$item->before($item2);

echo $doc->saveXML(), "\n";
?>
--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<element xmlns="http://www.w3.org/2005/Atom" xmlns:g="http://base.google.com/ns/1.0">
  <g:item_type>street</g:item_type>
  <g:item_type>house</g:item_type>
</element>
