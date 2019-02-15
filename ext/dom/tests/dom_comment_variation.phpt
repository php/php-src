--TEST--
DOM Comment : Variation
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php

$xml = <<< EOXML
<?xml version="1.0" encoding="ISO-8859-1"?><courses><!-- Hello World! --></courses>
EOXML;

$dom = new DOMDocument();
$dom->loadXML($xml);
$root = $dom->documentElement;
var_dump($root->hasChildNodes());
$children = $root->childNodes;

for ($index = 0; $index < $children->length; $index++) {
	echo "--- child $index ---\n";
	$current = $children->item($index);
	echo get_class($current), "\n";
	var_dump($current->textContent);
}
--EXPECT--
bool(true)
--- child 0 ---
DOMComment
string(14) " Hello World! "
