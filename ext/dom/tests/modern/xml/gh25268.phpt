--TEST--
GH-25268 (NULL php_libxml_ref_obj dereference in dom_objects_store_clone_obj when cloning Dom\Notation)
--EXTENSIONS--
dom
--CREDITS--
YuanchengJiang
--FILE--
<?php
$dom = Dom\XMLDocument::createFromString('<!DOCTYPE root [<!NOTATION GIF SYSTEM "image/gif">]><root/>');
$notation = $dom->doctype->notations->getNamedItem("GIF");

$clone = clone $notation;
var_dump($clone->nodeName, $clone->ownerDocument === $dom, $clone->parentNode, $clone->isConnected);

$clone2 = $notation->cloneNode();
var_dump($clone2->nodeName, $clone2 === $clone);

unset($dom, $notation);
var_dump($clone->nodeName, $clone2->nodeName);
?>
--EXPECT--
string(3) "GIF"
bool(true)
NULL
bool(false)
string(3) "GIF"
bool(false)
string(3) "GIF"
string(3) "GIF"
