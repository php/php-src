--TEST--
TokenList: entities interaction
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE root [
    <!ENTITY ent "foo">
]>
<root class="x&ent;x"/>
XML);
$element = $dom->documentElement;
$list = $element->classList;

var_dump($list);

var_dump($list->contains("xfoox"));
var_dump($list->contains("xx"));
var_dump($list->contains("foo"));

$list->add("test");

echo $dom->saveXML();

?>
--EXPECT--
object(Dom\TokenList)#3 (2) {
  ["length"]=>
  int(1)
  ["value"]=>
  string(5) "xfoox"
}
bool(true)
bool(false)
bool(false)
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
<!ENTITY ent "foo">
]>
<root class="xfoox test"/>
