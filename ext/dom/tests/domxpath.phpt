--TEST--
DOMXPath Tests
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

function MyAverage($nodelist) {
	$count = 0;
	$val = 0;
	foreach ($nodelist AS $node) {
		$count++;
		$val += $node->textContent;
	}
	if ($val > 0) {
		return $val/$count;
	} else {
		return 0;
	}
}

$dom = new DOMDocument;
$dom->loadXML(b'<root xmlns="urn::default"><child>myval</child></root>');

$xpath = new DOMXPath($dom);

$xpath->registerPHPFunctions('MyAverage');
$xpath->registerNamespace("php", "http://php.net/xpath");

$xpath->registerNamespace("def", "urn::default");
$nodelist = $xpath->query("//def:child");
if ($node = $nodelist->item(0)) {
	print $node->textContent."\n";
}

$count = $xpath->evaluate("count(//def:child)");

var_dump($count);

$xpathdoc = $xpath->document;

var_dump($xpathdoc instanceof DOMDocument);

$root = $dom->documentElement;
$root->appendChild($dom->createElementNS("urn::default", "testnode", 3));
$root->appendChild($dom->createElementNS("urn::default", "testnode", 4));
$root->appendChild($dom->createElementNS("urn::default", "testnode", 4));
$root->appendChild($dom->createElementNS("urn::default", "testnode", 5));

$avg = $xpath->evaluate('number(php:function("MyAverage", //def:testnode))');
var_dump($avg);
?>
--EXPECT--
myval
float(1)
bool(true)
float(4)