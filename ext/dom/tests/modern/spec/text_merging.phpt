--TEST--
Appending should not merge text nodes
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
$element = $dom->createElement("div");
$element->append("foo", "bar", "baz", $dom->createElement("span", "qux"), $dom->createCDATASection("a"), $dom->createCDATASection("b"));
$dom->append($element);
echo $dom->saveXML();

var_dump($element->firstChild->textContent);
var_dump($element->firstChild->nextSibling->textContent);
var_dump($element->firstChild->nextSibling->nextSibling->textContent);

$dom->normalize();

var_dump($element->firstChild->textContent);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<div>foobarbaz<span>qux</span><![CDATA[a]]><![CDATA[b]]></div>
string(3) "foo"
string(3) "bar"
string(3) "baz"
string(9) "foobarbaz"
