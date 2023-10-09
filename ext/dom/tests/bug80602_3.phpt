--TEST--
Bug #80602 (Segfault when using DOMChildNode::before()) - use-after-free variation
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
$target->before('bar', $doc->documentElement->firstChild, 'baz');
echo $doc->saveXML($doc->documentElement), "\n";
var_dump($target);

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/></a>');
$target = $doc->documentElement->lastChild;
// Note: after instead of before
$target->after('bar', $doc->documentElement->firstChild, 'baz');
echo $doc->saveXML($doc->documentElement), "\n";
var_dump($target);

?>
--EXPECTF--
<a>barfoobaz<last/></a>
object(DOMElement)#3 (27) {
  ["schemaTypeInfo"]=>
  NULL
  ["tagName"]=>
  string(4) "last"
  ["className"]=>
  string(0) ""
  ["id"]=>
  string(0) ""
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
  string(4) "last"
  ["nodeValue"]=>
  string(0) ""
  ["nodeType"]=>
  int(1)
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  string(22) "(object value omitted)"
  ["nextSibling"]=>
  NULL
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  string(4) "last"
  ["baseURI"]=>
  string(%d) %s
  ["textContent"]=>
  string(0) ""
}
<a><last/>barfoobaz</a>
object(DOMElement)#2 (27) {
  ["schemaTypeInfo"]=>
  NULL
  ["tagName"]=>
  string(4) "last"
  ["className"]=>
  string(0) ""
  ["id"]=>
  string(0) ""
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
  string(4) "last"
  ["nodeValue"]=>
  string(0) ""
  ["nodeType"]=>
  int(1)
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
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
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  string(4) "last"
  ["baseURI"]=>
  string(%d) %s
  ["textContent"]=>
  string(0) ""
}
