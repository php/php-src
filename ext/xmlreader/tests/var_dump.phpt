--TEST--
XMLReader - var_dump
--EXTENSIONS--
xmlreader
--FILE--
<?php
$reader = XMLReader::fromString("<root>hi</root>");
var_dump($reader->read());
var_dump($reader);
?>
--EXPECTF--
bool(true)
object(XMLReader)#%d (14) {
  ["attributeCount"]=>
  int(0)
  ["baseURI"]=>
  string(%d) "%s"
  ["depth"]=>
  int(0)
  ["hasAttributes"]=>
  bool(false)
  ["hasValue"]=>
  bool(false)
  ["isDefault"]=>
  bool(false)
  ["isEmptyElement"]=>
  bool(false)
  ["localName"]=>
  string(4) "root"
  ["name"]=>
  string(4) "root"
  ["namespaceURI"]=>
  string(0) ""
  ["nodeType"]=>
  int(1)
  ["prefix"]=>
  string(0) ""
  ["value"]=>
  string(0) ""
  ["xmlLang"]=>
  string(0) ""
}
