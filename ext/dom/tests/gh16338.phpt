--TEST--
GH-16338 (Null-dereference in ext/dom/node.c)
--EXTENSIONS--
dom
--CREDITS--
chibinz
--FILE--
<?php
$ref = new DOMEntityReference("G");
$com = new DOMComment();
$doc = new DOMDocument();
$elem = new DOMElement("Rj", "o");
$com2 = new DOMComment();
$elem2 = new DOMElement("kx", null, "r");

$elem2->prepend($com);
$com->before("Z");
$com->before($com2);
$com2->after($elem);
$doc->insertBefore($elem2);
$elem->insertBefore($ref);

echo $doc->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<kx xmlns="r">Z<Rj>o&G;</Rj></kx>
