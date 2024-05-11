--TEST--
Freeing of a predefined DOMEntityReference
--EXTENSIONS--
dom
--FILE--
<?php
$ref = new DOMEntityReference("amp");
var_dump($ref);
?>
--EXPECT--
object(DOMEntityReference)#1 (17) {
  ["nodeName"]=>
  string(3) "amp"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(5)
  ["parentNode"]=>
  NULL
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
  bool(false)
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
