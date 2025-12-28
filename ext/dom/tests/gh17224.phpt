--TEST--
GH-17224 (UAF in importNode)
--EXTENSIONS--
dom
--CREDITS--
YuanchengJiang
--FILE--
<?php
$aDOM = new DOMDocument();
$fromdom = new DOMDocument();
$fromdom->loadXML('<data xmlns:ai="http://test.org" ai:attr="namespaced" />');
$attr = $fromdom->firstChild->attributes->item(0);
$att = $aDOM->importNode($attr);
$doc = new DOMDocument;
$fromdom->load(__DIR__."/book.xml");
unset($attr);
var_dump($att);
?>
--EXPECTF--
object(DOMAttr)#%d (%d) {
  ["name"]=>
  string(4) "attr"
  ["specified"]=>
  bool(true)
  ["value"]=>
  string(10) "namespaced"
  ["ownerElement"]=>
  NULL
  ["schemaTypeInfo"]=>
  NULL
  ["nodeName"]=>
  string(7) "ai:attr"
  ["nodeValue"]=>
  string(10) "namespaced"
  ["nodeType"]=>
  int(2)
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
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  string(15) "http://test.org"
  ["prefix"]=>
  string(2) "ai"
  ["localName"]=>
  string(4) "attr"
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(10) "namespaced"
}
