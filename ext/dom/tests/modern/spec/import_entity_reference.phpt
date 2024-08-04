--TEST--
Importing an entity reference without also importing the document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
    <!ENTITY foo "bar">
]>
<root>&foo;</root>
XML);

$importer = Dom\XMLDocument::createEmpty();
$imported = $importer->importNode($dom->documentElement, true);
echo $importer->saveXml($imported), "\n";

var_dump($imported->firstChild);

?>
--EXPECT--
<root>&foo;</root>
object(Dom\EntityReference)#3 (14) {
  ["nodeType"]=>
  int(5)
  ["nodeName"]=>
  string(3) "foo"
  ["baseURI"]=>
  string(11) "about:blank"
  ["isConnected"]=>
  bool(false)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  NULL
}
