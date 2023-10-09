--TEST--
Clone nodes
--EXTENSIONS--
dom
--FILE--
<?php

echo "-- Clone DOMNameSpaceNode --\n";

$doc = new DOMDocument;
$doc->loadXML('<foo xmlns="http://php.net/test" xmlns:foo="urn:foo" />');

$attr = $doc->documentElement->getAttributeNode('xmlns');
var_dump($attr);

$attrClone = clone $attr;
var_dump($attrClone->nodeValue);
var_dump($attrClone->parentNode->nodeName);

unset($doc);
unset($attr);

var_dump($attrClone->nodeValue);
var_dump($attrClone->parentNode->nodeName);

echo "-- Clone DOMNode --\n";

$doc = new DOMDocument;
$doc->loadXML('<foo><bar/></foo>');

$bar = $doc->documentElement->firstChild;
$barClone = clone $bar;
$bar->remove();
unset($bar);

var_dump($barClone->nodeName);

$doc->firstElementChild->remove();
unset($doc);

var_dump($barClone->nodeName);
var_dump($barClone->parentNode);

?>
--EXPECT--
-- Clone DOMNameSpaceNode --
object(DOMNameSpaceNode)#3 (10) {
  ["nodeName"]=>
  string(5) "xmlns"
  ["nodeValue"]=>
  string(19) "http://php.net/test"
  ["nodeType"]=>
  int(18)
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  string(5) "xmlns"
  ["namespaceURI"]=>
  string(19) "http://php.net/test"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
}
string(19) "http://php.net/test"
string(3) "foo"
string(19) "http://php.net/test"
string(3) "foo"
-- Clone DOMNode --
string(3) "bar"
string(3) "bar"
NULL
