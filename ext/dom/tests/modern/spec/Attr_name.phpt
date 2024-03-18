--TEST--
DOMAttr::$name
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$foo = $dom->appendChild($dom->createElement("foo"));
$foo->setAttributeNS("http://example.com", "foo:bar", "baz");
var_dump($foo->attributes[0]->name);

?>
--EXPECT--
string(7) "foo:bar"
