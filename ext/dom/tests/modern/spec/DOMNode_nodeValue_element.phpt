--TEST--
DOMNode::$nodeValue on an element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$foo = $dom->createElement("foo");
$foo->textContent = "bar";
var_dump($foo->nodeValue);

?>
--EXPECT--
NULL
