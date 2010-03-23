--TEST--
DOM Comment : Basic Functionality
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php

$xml = <<< EOXML
<?xml version="1.0" encoding="ISO-8859-1"?>
<courses>
	<!-- Hello World! -->
</courses>
EOXML;

$dom = new DOMDocument();
$dom->loadXML($xml);
$root = $dom->documentElement;
var_dump($root->hasChildNodes());
$children = $root->childNodes;

for ($index = 0; $index < $children->length; $index++) {
	echo "--- child $index ---\n";
	$current = $children->item($index);
	var_dump($current);
	var_dump($current->textContent);
}

--EXPECTF--
bool(true)
--- child 0 ---
object(DOMText)#%d (0) {
}
string(2) "
	"
--- child 1 ---
object(DOMComment)#%d (0) {
}
string(14) " Hello World! "
--- child 2 ---
object(DOMText)#%d (0) {
}
string(1) "
"

