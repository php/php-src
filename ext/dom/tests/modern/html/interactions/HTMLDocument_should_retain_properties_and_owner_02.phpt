--TEST--
DOM\HTMLDocument should retain properties and ownerDocument relation 02
--EXTENSIONS--
dom
--FILE--
<?php

class MyElement extends DOM\Element {}

$dom = DOM\HTMLDocument::createFromString("<p>foo</p>", LIBXML_NOERROR);
$dom->registerNodeClass("DOM\\Element", "MyElement");
$child = $dom->documentElement->appendChild($dom->createElement('html'));

// Destroy reference to the DOM
unset($dom);

// Regain reference using the ownerDocument property
// Should be a DOM\HTMLDocument
$dom = $child->ownerDocument;
var_dump($dom);
// Test if property is preserved (any random doc_props property will do)
var_dump(get_class($dom->getElementsByTagName("p")->item(0)));

?>
--EXPECT--
object(DOM\HTMLDocument)#1 (26) {
  ["implementation"]=>
  string(22) "(object value omitted)"
  ["URL"]=>
  string(11) "about:blank"
  ["documentURI"]=>
  string(11) "about:blank"
  ["characterSet"]=>
  string(5) "UTF-8"
  ["charset"]=>
  string(5) "UTF-8"
  ["inputEncoding"]=>
  string(5) "UTF-8"
  ["doctype"]=>
  NULL
  ["documentElement"]=>
  string(22) "(object value omitted)"
  ["firstElementChild"]=>
  string(22) "(object value omitted)"
  ["lastElementChild"]=>
  string(22) "(object value omitted)"
  ["childElementCount"]=>
  int(1)
  ["body"]=>
  string(22) "(object value omitted)"
  ["nodeType"]=>
  int(13)
  ["nodeName"]=>
  string(9) "#document"
  ["baseURI"]=>
  string(11) "about:blank"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  NULL
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  NULL
}
string(9) "MyElement"
