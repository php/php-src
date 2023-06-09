--TEST--
DOMXPath::query() can return DOMNodeList with DOMNameSpaceNode items
--EXTENSIONS--
dom
--FILE--
<?php

$xml = <<<'XML'
<?xml version="1.0" encoding="UTF-8"?>
<test></test>
XML;
$doc = new DomDocument;
$doc->loadXML($xml);
$xpath = new DOMXPath($doc);
$nodes = $xpath->query('//namespace::*');
var_dump($nodes->item(0));

?>
--EXPECT--
object(DOMNameSpaceNode)#4 (8) {
  ["nodeName"]=>
  string(9) "xmlns:xml"
  ["nodeValue"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["nodeType"]=>
  int(18)
  ["prefix"]=>
  string(3) "xml"
  ["localName"]=>
  string(3) "xml"
  ["namespaceURI"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
}
