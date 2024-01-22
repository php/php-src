--TEST--
Objects of DOM extension: debug info object handler.
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<XML
<foo>
    <bar>foobar</bar>
</foo>
XML;
$d = new domdocument;
$d->dynamicProperty = new stdclass;
$d->loadXML($xml);
var_dump($d);
?>
--EXPECTF--
Deprecated: Creation of dynamic property DOMDocument::$dynamicProperty is deprecated in %s on line %d
object(DOMDocument)#1 (41) {
  ["dynamicProperty"]=>
  object(stdClass)#2 (0) {
  }
  ["implementation"]=>
  string(22) "(object value omitted)"
  ["actualEncoding"]=>
  NULL
  ["config"]=>
  NULL
  ["encoding"]=>
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
  ["doctype"]=>
  NULL
  ["documentElement"]=>
  string(22) "(object value omitted)"
  ["strictErrorChecking"]=>
  bool(true)
  ["documentURI"]=>
  string(%d) "%s"
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
  string(%d) %s
  ["textContent"]=>
  string(12) "
    foobar
"
}
