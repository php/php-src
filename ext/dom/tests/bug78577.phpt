--TEST--
Bug #78577 (Crash in DOMNameSpace debug info handlers)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->loadXML('<foo xmlns="http://php.net/test" xmlns:foo="urn:foo" />');

$attr = $doc->documentElement->getAttributeNode('xmlns');
var_dump($attr);

?>
--EXPECT--
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
