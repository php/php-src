--TEST--
DOMDocument::removeChild: The node cannot be removed permanently
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$dom = new DOMDocument();
$root = $dom->createElement('html');
$dom->appendChild($root);

$el1 = $dom->createElement('p1');
$el1->setAttribute('id', 'foo');
$el1->setIdAttribute('id', true);

$root->appendChild($el1);

$el2 = $dom->createElement('p2');
$el2->setAttribute('id', 'bar');
$el2->setIdAttribute('id', true);

$root->appendChild($el2);

var_dump($dom->getElementById('foo'));
$root->removeChild($dom->getElementById('foo'));
var_dump($dom->getElementById('bar'));
var_dump($dom->getElementById('foo'));
echo $dom->saveXML();
?>
--EXPECTF--
object(DOMElement)#%d (%d) {
  ["tagName"]=>
  string(2) "p1"
  ["schemaTypeInfo"]=>
  NULL
  ["firstElementChild"]=>
  NULL
  ["lastElementChild"]=>
  NULL
  ["childElementCount"]=>
  int(0)
  ["previousElementSibling"]=>
  NULL
  ["nextElementSibling"]=>
  string(22) "(object value omitted)"
  ["nodeName"]=>
  string(2) "p1"
  ["nodeValue"]=>
  string(0) ""
  ["nodeType"]=>
  int(1)
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  string(22) "(object value omitted)"
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  string(2) "p1"
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
object(DOMElement)#%d (%d) {
  ["tagName"]=>
  string(2) "p2"
  ["schemaTypeInfo"]=>
  NULL
  ["firstElementChild"]=>
  NULL
  ["lastElementChild"]=>
  NULL
  ["childElementCount"]=>
  int(0)
  ["previousElementSibling"]=>
  NULL
  ["nextElementSibling"]=>
  NULL
  ["nodeName"]=>
  string(2) "p2"
  ["nodeValue"]=>
  string(0) ""
  ["nodeType"]=>
  int(1)
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  NULL
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  string(2) "p2"
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
NULL
<?xml version="1.0"?>
<html><p2 id="bar"/></html>
