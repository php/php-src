--TEST--
Bug #39760 (autocreating element doesn't assign value to first node)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip simplexml extension is not loaded"; ?>
--FILE--
<?php

$sx1 = new SimpleXMLElement("<root />");

$sx1->node[0] = 'node1';
$sx1->node[1] = 'node2';

print $sx1->asXML()."\n";
$node = $sx1->node[0];
$node[0] = 'New Value';

print $sx1->asXML();

?>
--EXPECTF--
<?xml version="1.0"?>
<root><node>node1</node><node>node2</node></root>

<?xml version="1.0"?>
<root><node>New Value</node><node>node2</node></root>
