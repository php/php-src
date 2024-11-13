--TEST--
compareDocumentPosition: contains attribute for a freestanding element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$foo = $dom->createElement("foo");
$foo->setAttribute("test", "value");
$attribute = $foo->attributes[0];

echo $dom->saveXML($foo), "\n";

var_dump($foo->compareDocumentPosition($attribute) === (DOMNode::DOCUMENT_POSITION_FOLLOWING | DOMNode::DOCUMENT_POSITION_CONTAINED_BY));
var_dump($attribute->compareDocumentPosition($foo) === (DOMNode::DOCUMENT_POSITION_PRECEDING | DOMNode::DOCUMENT_POSITION_CONTAINS));

?>
--EXPECT--
<foo test="value"/>
bool(true)
bool(true)
