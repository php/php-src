--TEST--
DOMNode::C14N()
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php
$xml = <<< XML
<?xml version="1.0" ?>
<books>
 <book>
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book>
 <book>
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
</books>
XML;

$doc = new DOMDocument();
$doc->loadXML($xml);
$node = $doc->getElementsByTagName('title')->item(0);
var_dump($node->C14N());
?>
--EXPECTF--
string(34) "<title>The Grapes of Wrath</title>"
