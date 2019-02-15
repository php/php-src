--TEST--
Bug #44478 (Inconsistent behaviour when assigning new nodes)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml_element = new simpleXMLElement('<root></root>');
$xml_element->node1 = 'a &#38; b';
print $xml_element->node1."\n";
$xml_element->node1 = 'a &#38; b';
print $xml_element->node1."\n";
$xml_element->addChild('node2','a &#38; b');
print $xml_element->node2."\n";
$xml_element->node2 = 'a & b';
print $xml_element->node2."\n";

print $xml_element->asXML();

?>
===DONE===
--EXPECT--
a &#38; b
a &#38; b
a & b
a & b
<?xml version="1.0"?>
<root><node1>a &amp;#38; b</node1><node2>a &amp; b</node2></root>
===DONE===
	
