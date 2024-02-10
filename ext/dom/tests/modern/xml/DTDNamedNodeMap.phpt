--TEST--
DTDNamedNodeMap
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<!DOCTYPE root [
<!ENTITY test "entity is only for test purposes">
<!ENTITY myimage PUBLIC "-" "mypicture.gif" NDATA GIF>
<!NOTATION GIF SYSTEM "viewgif.exe">
]>
<root/>
XML);

$doctype = $dom->doctype;

var_dump($doctype);

var_dump($doctype->entities["test"]);
var_dump($doctype->entities["myimage"]);
// TODO: isConnected returning false is a bug
var_dump($doctype->notations["GIF"]);

?>
--EXPECTF--
object(DOM\DocumentType)#2 (24) {
  ["name"]=>
  string(4) "root"
  ["entities"]=>
  string(22) "(object value omitted)"
  ["notations"]=>
  string(22) "(object value omitted)"
  ["publicId"]=>
  string(0) ""
  ["systemId"]=>
  string(0) ""
  ["internalSubset"]=>
  string(105) "<!ENTITY test "entity is only for test purposes">
<!ENTITY myimage PUBLIC "-" "mypicture.gif" NDATA GIF>
"
  ["nodeName"]=>
  string(4) "root"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(10)
  ["parentNode"]=>
  string(22) "(object value omitted)"
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
  string(22) "(object value omitted)"
  ["attributes"]=>
  NULL
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  string(%d) "%s"
  ["textContent"]=>
  NULL
}
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
  string(4) "test"
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
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  string(22) "(object value omitted)"
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  NULL
}
object(DOM\Entity)#3 (17) {
  ["publicId"]=>
  string(1) "-"
  ["systemId"]=>
  string(13) "mypicture.gif"
  ["notationName"]=>
  string(3) "GIF"
  ["nodeType"]=>
  int(17)
  ["nodeName"]=>
  string(7) "myimage"
  ["baseURI"]=>
  string(%d) "%smypicture.gif"
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
  NULL
  ["lastChild"]=>
  NULL
  ["previousSibling"]=>
  string(22) "(object value omitted)"
  ["nextSibling"]=>
  NULL
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  NULL
}
object(DOM\Notation)#4 (13) {
  ["nodeType"]=>
  int(12)
  ["nodeName"]=>
  string(3) "GIF"
  ["baseURI"]=>
  NULL
  ["isConnected"]=>
  bool(false)
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
  string(0) ""
}
