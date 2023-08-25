--TEST--
HTML5 document should retain properties and ownerDocument relation 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->strictErrorChecking = false;
$child = $dom->appendChild($dom->createElement('html'));

// Destroy reference to the DOM
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
  NULL
  ["doctype"]=>
  NULL
  ["implementation"]=>
  string(22) "(object value omitted)"
  ["documentElement"]=>
  string(22) "(object value omitted)"
  ["actualEncoding"]=>
  NULL
  ["xmlEncoding"]=>
  NULL
  ["standalone"]=>
  bool(false)
  ["xmlStandalone"]=>
  bool(false)
  ["version"]=>
  string(3) "1.0"
  ["xmlVersion"]=>
  string(3) "1.0"
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
  int(9)
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
  string(0) ""
}
bool(false)
