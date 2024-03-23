--TEST--
DOM\XMLDocument debug
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
var_dump($dom);

?>
--EXPECT--
object(DOM\XMLDocument)#1 (30) {
  ["xmlEncoding"]=>
  string(5) "UTF-8"
  ["xmlStandalone"]=>
  bool(false)
  ["xmlVersion"]=>
  string(3) "1.0"
  ["formatOutput"]=>
  bool(false)
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
  NULL
  ["firstElementChild"]=>
  NULL
  ["lastElementChild"]=>
  NULL
  ["childElementCount"]=>
  int(0)
  ["body"]=>
  NULL
  ["nodeType"]=>
  int(9)
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
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  NULL
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  NULL
}
