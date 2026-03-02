--TEST--
compareDocumentPosition: equal nodes
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
<p>foo</p>
<p>foo</p>
</container>
XML);

$xpath = new DOMXPath($dom);
$nodes = $xpath->query('//p');
var_dump($nodes->item(0)->compareDocumentPosition($nodes->item(0)) === 0);
var_dump($nodes->item(1)->compareDocumentPosition($nodes->item(1)) === 0);
var_dump($nodes->item(0)->compareDocumentPosition($nodes->item(1)) === 0);
var_dump($nodes->item(1)->compareDocumentPosition($nodes->item(0)) === 0);

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
