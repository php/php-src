--TEST--
compareDocumentPosition: direct root children
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$foo = $dom->appendChild($dom->createElement("foo"));
$bar = $dom->appendChild($dom->createElement("bar"));

var_dump($foo->compareDocumentPosition($bar) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
var_dump($bar->compareDocumentPosition($foo) === DOMNode::DOCUMENT_POSITION_PRECEDING);

?>
--EXPECT--
bool(true)
bool(true)
