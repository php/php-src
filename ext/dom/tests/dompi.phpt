--TEST--
DOMProcessingInstruction Tests
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML(b'<root />');

$pi = new DOMProcessingInstruction("mypi", "my instructions");

$dom->documentElement->appendChild($pi);

print $pi->target."\n";
print $pi->data."\n";
$pi->data = "my new instructions";

print $dom->saveXML();
?>
--EXPECT--
mypi
my instructions
<?xml version="1.0"?>
<root><?mypi my new instructions?></root>
