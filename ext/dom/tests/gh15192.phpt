--TEST--
GH-15192 (Segmentation fault in dom extension (html5_serializer))
--EXTENSIONS--
dom
--FILE--
<?php
$dom = @Dom\HTMLDocument::createFromString("<p>foo</p>");
$dom2 = clone $dom;
$element = $dom2->firstChild;
$dom = new DomDocument();
var_dump($element);
?>
--EXPECTF--
object(Dom\HTMLElement)#3 (29) {
  ["namespaceURI"]=>
  string(28) "http://www.w3.org/1999/xhtml"
  ["prefix"]=>
  NULL
  ["localName"]=>
  string(4) "html"
  ["tagName"]=>
  string(4) "HTML"
  ["id"]=>
  string(0) ""
  ["className"]=>
  string(0) ""
  ["classList"]=>
  string(22) "(object value omitted)"
  ["attributes"]=>
  string(22) "(object value omitted)"
  ["firstElementChild"]=>
  string(22) "(object value omitted)"
  ["lastElementChild"]=>
  string(22) "(object value omitted)"
  ["childElementCount"]=>
  int(2)
  ["previousElementSibling"]=>
  NULL
  ["nextElementSibling"]=>
  NULL
  ["innerHTML"]=>
  string(36) "<head></head><body><p>foo</p></body>"
  ["substitutedNodeValue"]=>
  string(3) "foo"
  ["nodeType"]=>
  int(1)
  ["nodeName"]=>
  string(4) "HTML"
  ["baseURI"]=>
  string(11) "about:blank"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  NULL
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  string(22) "(object value omitted)"
  ["lastChild"]=>
  string(22) "(object value omitted)"
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  NULL
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(3) "foo"
}
