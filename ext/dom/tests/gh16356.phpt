--TEST--
GH-16356 (Segmentation fault with $outerHTML and next node)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$dom->append($dom->createElement("container"));
$e1 = $dom->documentElement->appendChild($dom->createElementNS("urn:example1", "example:foo"));
$e2 = $dom->documentElement->appendChild($dom->createElementNS("urn:example2", "example:foo"));
var_dump($e1, $e2);

?>
--EXPECT--
object(Dom\Element)#3 (31) {
  ["namespaceURI"]=>
  string(12) "urn:example1"
  ["prefix"]=>
  string(7) "example"
  ["localName"]=>
  string(3) "foo"
  ["tagName"]=>
  string(11) "example:foo"
  ["id"]=>
  string(0) ""
  ["className"]=>
  string(0) ""
  ["classList"]=>
  string(22) "(object value omitted)"
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["children"]=>
  string(22) "(object value omitted)"
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
  ["innerHTML"]=>
  string(0) ""
  ["outerHTML"]=>
  string(27) "<example:foo></example:foo>"
  ["substitutedNodeValue"]=>
  string(0) ""
  ["nodeType"]=>
  int(1)
  ["nodeName"]=>
  string(11) "example:foo"
  ["baseURI"]=>
  string(11) "about:blank"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
object(Dom\Element)#4 (31) {
  ["namespaceURI"]=>
  string(12) "urn:example2"
  ["prefix"]=>
  string(7) "example"
  ["localName"]=>
  string(3) "foo"
  ["tagName"]=>
  string(11) "example:foo"
  ["id"]=>
  string(0) ""
  ["className"]=>
  string(0) ""
  ["classList"]=>
  string(22) "(object value omitted)"
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["children"]=>
  string(22) "(object value omitted)"
  ["firstElementChild"]=>
  NULL
  ["lastElementChild"]=>
  NULL
  ["childElementCount"]=>
  int(0)
  ["previousElementSibling"]=>
  string(22) "(object value omitted)"
  ["nextElementSibling"]=>
  NULL
  ["innerHTML"]=>
  string(0) ""
  ["outerHTML"]=>
  string(27) "<example:foo></example:foo>"
  ["substitutedNodeValue"]=>
  string(0) ""
  ["nodeType"]=>
  int(1)
  ["nodeName"]=>
  string(11) "example:foo"
  ["baseURI"]=>
  string(11) "about:blank"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
