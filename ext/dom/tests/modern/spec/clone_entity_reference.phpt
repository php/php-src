--TEST--
Cloning an entity reference without also cloning the document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
    <!ENTITY foo "bar">
]>
<root>&foo;</root>
XML);

$clone = $dom->documentElement->cloneNode(true);
echo $dom->saveXML($clone), "\n";

var_dump($clone->firstChild->firstChild);

?>
--EXPECT--
<root>&foo;</root>
object(DOMEntity)#4 (24) {
  ["actualEncoding"]=>
  NULL
  ["encoding"]=>
  NULL
  ["version"]=>
  NULL
  ["publicId"]=>
  NULL
  ["systemId"]=>
  NULL
  ["notationName"]=>
  NULL
  ["nodeName"]=>
  string(3) "foo"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(17)
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
  ["attributes"]=>
  NULL
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
