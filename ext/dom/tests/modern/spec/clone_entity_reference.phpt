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
object(DOM\Entity)#4 (17) {
  ["publicId"]=>
  NULL
  ["systemId"]=>
  NULL
  ["notationName"]=>
  NULL
  ["nodeType"]=>
  int(17)
  ["nodeName"]=>
  string(3) "foo"
  ["baseURI"]=>
  NULL
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
  NULL
}
