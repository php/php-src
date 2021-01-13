--TEST--
Bug #78577 DOMNameSpaceNode::parentNode on xmlns segfaults
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML(<<<XML
    <foo xmlns="http://php.net/test" xmlns:foo="urn:foo" foo="bar">
        <bar baz="boing" />
    </foo>
XML
);

$attr1 = $doc->documentElement->getAttributeNode("foo");
$element = $doc->documentElement->getElementsByTagName("bar")[0];
$attr2 = $element->getAttributeNode("baz");
$attrXmlnsFoo = $doc->documentElement->getAttributeNode("xmlns:foo");
$attrXmlns = $doc->documentElement->getAttributeNode('xmlns');

if ($attr1->parentNode === $doc->documentElement) {
    echo "attr1\n";
}

if ($attr2->parentNode === $element) {
    echo "attr2\n";
}

if ($attrXmlnsFoo->parentNode === $doc->documentElement) {
    echo "attrXmlnsFoo\n";
}

if ($attrXmlns->parentNode === $doc->documentElement) {
    echo "attrXmlns\n";
}

var_dump($attrXmlns);

--EXPECTF--
attr1
attr2
attrXmlnsFoo
attrXmlns
object(DOMNameSpaceNode)#7 (8) {
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
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
}
