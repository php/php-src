--TEST--
Element renaming interaction with the HTML namespace 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
$el = $dom->createElementNS("http://www.w3.org/1999/xhtml", "foo:bar");
$el->rename("http://www.w3.org/1999/xhtml", "foo:baz");
var_dump($el->nodeName, $el->namespaceURI, $el->prefix);

// Very subtly *not* the HTML namespace!
$el = $dom->createElementNS("http://www.w3.org/1999/xhtml/", "foo:bar");
$el->rename("urn:x", "foo:baz");
var_dump($el->nodeName, $el->namespaceURI, $el->prefix);

?>
--EXPECT--
string(7) "foo:baz"
string(28) "http://www.w3.org/1999/xhtml"
string(3) "foo"
string(7) "foo:baz"
string(5) "urn:x"
string(3) "foo"
