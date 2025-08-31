--TEST--
Text coalesce bug when appending fragment with text nodes
--EXTENSIONS--
dom
--FILE--
<?php
$document = new DOMDocument();
$document->loadXML('<root/>');

$sut = $document->createDocumentFragment();
for($i = 0; $i < 10; $i++) {
	$textNode = $document->createTextNode("Node$i");
	$sut->append($textNode);
}

$document->documentElement->append($sut);
echo $document->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<root>Node0Node1Node2Node3Node4Node5Node6Node7Node8Node9</root>
