--TEST--
Fix GH-11682 (Consecutive calls to DOMNode::before and DOMNode::remove unlink wrong nodes)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<?xml version="1.0" ?><root><node /></root>');

$list = [
    $dom->createElement('foo1'),
    $dom->createElement('foo2'),
    $dom->createElement('foo3')
];

$node = $dom->getElementsByTagName('node')->item(0);

$node->before(...$list);
$node->remove();

echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<root><foo1/><foo2/><foo3/></root>
