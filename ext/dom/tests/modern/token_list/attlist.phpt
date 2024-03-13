--TEST--
TokenList: ATTLIST interaction
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE root [
<!ELEMENT root EMPTY>
<!ATTLIST child class CDATA "first second">
]>
<root><child/></root>
XML, LIBXML_DTDATTR);
$element = $dom->documentElement->firstChild;
$list = $element->classList;

echo 'class: ', $element->getAttribute('class'), "\n";
var_dump($list);

$list->remove('first');

var_dump($list);
echo $dom->saveXML(), "\n";

?>
--EXPECT--
class: first second
object(DOM\TokenList)#2 (2) {
  ["length"]=>
  int(2)
  ["value"]=>
  string(12) "first second"
}
object(DOM\TokenList)#2 (2) {
  ["length"]=>
  int(1)
  ["value"]=>
  string(6) "second"
}
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
<!ELEMENT root EMPTY>
<!ATTLIST child class CDATA "first second">
]>
<root><child class="second"/></root>
