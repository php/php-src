--TEST--
HTML5 document should retain properties and ownerDocument relation 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->strictErrorChecking = false;
$dom->loadHTML("<p>foo</p>", LIBXML_NOERROR);

// Destroy reference to the DOM
$child = $dom->documentElement;
unset($dom);

// Regain reference using the ownerDocument property
// Should be a DOM\HTML5Document
$dom = $child->ownerDocument;
var_dump($dom);
// Test if property is preserved (any random doc_props property will do)
var_dump($dom->strictErrorChecking);

?>
--EXPECT--
object(DOM\HTML5Document)#1 (40) {
  ["encoding"]=>
  string(5) "UTF-8"
  ["doctype"]=>
  NULL
  ["implementation"]=>
  string(22) "(object value omitted)"
  ["documentElement"]=>
  string(22) "(object value omitted)"
  ["actualEncoding"]=>
  string(5) "UTF-8"
  ["xmlEncoding"]=>
  string(5) "UTF-8"
  ["standalone"]=>
  bool(true)
  ["xmlStandalone"]=>
  bool(true)
  ["version"]=>
  NULL
  ["xmlVersion"]=>
  NULL
  ["strictErrorChecking"]=>
  bool(false)
  ["documentURI"]=>
  NULL
  ["config"]=>
  NULL
  ["formatOutput"]=>
  bool(false)
  ["validateOnParse"]=>
  bool(false)
  ["resolveExternals"]=>
  bool(false)
  ["preserveWhiteSpace"]=>
  bool(true)
  ["recover"]=>
  bool(false)
  ["substituteEntities"]=>
  bool(false)
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
bool(false)
