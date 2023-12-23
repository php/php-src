--TEST--
Importing an entity reference without also importing the document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
    <!ENTITY foo "bar">
]>
<root>&foo;</root>
XML);

$importer = DOM\XMLDocument::createEmpty();
$imported = $importer->importNode($dom->documentElement, true);
echo $importer->saveXML($imported), "\n";

var_dump($imported->firstChild);

?>
--EXPECT--
<root>&foo;</root>
object(DOMEntityReference)#3 (18) {
  ["nodeName"]=>
  string(3) "foo"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(5)
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
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
  ["attributes"]=>
  NULL
  ["isConnected"]=>
  bool(false)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
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
