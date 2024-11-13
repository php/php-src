--TEST--
Dom\HTMLDocument should retain properties and ownerDocument relation 01
--EXTENSIONS--
dom
--FILE--
<?php

class MyElement extends Dom\HTMLElement {}

$dom = Dom\HTMLDocument::createFromString("<p>foo</p>", LIBXML_NOERROR);
$dom->registerNodeClass("Dom\\HTMLElement", "MyElement");

// Destroy reference to the DOM
$child = $dom->documentElement;
unset($dom);

// Regain reference using the ownerDocument property
// Should be a Dom\HTMLDocument
$dom = $child->ownerDocument;
var_dump($dom);
// Test if property is preserved (any random doc_props property will do)
var_dump(get_class($dom->getElementsByTagName("p")->item(0)));

?>
--EXPECT--
object(Dom\HTMLDocument)#1 (28) {
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
  ["head"]=>
  string(22) "(object value omitted)"
  ["title"]=>
  string(0) ""
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
