--TEST--
DOMXPath Tests
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML(b'<root xmlns="urn::default"><child>myval</child></root>');

$xpath = new DOMXPath($dom);

$xpath->registerNamespace("def", "urn::default");
$nodelist = $xpath->query("//def:child");
if ($node = $nodelist->item(0)) {
	print $node->textContent."\n";
}

$count = $xpath->evaluate("count(//def:child)");

var_dump($count);

$xpathdoc = $xpath->document;

var_dump($xpathdoc instanceof DOMDocument);
?>
--EXPECT--
myval
float(1)
bool(true)
