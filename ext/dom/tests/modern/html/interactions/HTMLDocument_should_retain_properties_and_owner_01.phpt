--TEST--
DOM\HTMLDocument should retain properties and ownerDocument relation 01
--EXTENSIONS--
dom
--FILE--
<?php

class MyElement extends DOM\Element {}

$dom = DOM\HTMLDocument::createFromString("<p>foo</p>", LIBXML_NOERROR);
$dom->registerNodeClass("DOMElement", "MyElement");

// Destroy reference to the DOM
$child = $dom->documentElement;
unset($dom);

// Regain reference using the ownerDocument property
// Should be a DOM\HTML5Document
$dom = $child->ownerDocument;
var_dump($dom);
// Test if property is preserved (any random doc_props property will do)
var_dump(get_class($dom->getElementsByTagName("p")->item(0)));

?>
--EXPECT--
object(DOM\HTMLDocument)#1 (25) {
  ["encoding"]=>
  string(5) "UTF-8"
  ["doctype"]=>
  NULL
  ["documentElement"]=>
  string(22) "(object value omitted)"
  ["documentURI"]=>
  NULL
  ["firstElementChild"]=>
  string(22) "(object value omitted)"
  ["lastElementChild"]=>
  string(22) "(object value omitted)"
  ["childElementCount"]=>
  int(1)
  ["nodeName"]=>
  string(9) "#document"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(13)
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
  bool(true)
  ["ownerDocument"]=>
  NULL
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(3) "foo"
}
string(9) "MyElement"
