--TEST--
Appending should not merge text nodes
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();
$element = $dom->createElement("div");
$span = $dom->createElement("span");
$span->textContent = "qux";
$element->append("foo", "bar", "baz", $span, $dom->createCDATASection("a"), $dom->createCDATASection("b"));
$dom->append($element);
echo $dom->saveXml(), "\n";

var_dump($element->firstChild->textContent);
var_dump($element->firstChild->nextSibling->textContent);
var_dump($element->firstChild->nextSibling->nextSibling->textContent);

$dom->normalize();
echo $dom->saveXml(), "\n";

var_dump($element->firstChild->textContent);
var_dump($element->firstChild->nextSibling->textContent);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<div>foobarbaz<span>qux</span><![CDATA[a]]><![CDATA[b]]></div>
string(3) "foo"
string(3) "bar"
string(3) "baz"
<?xml version="1.0" encoding="UTF-8"?>
<div>foobarbaz<span>qux</span><![CDATA[a]]><![CDATA[b]]></div>
string(9) "foobarbaz"
string(3) "qux"
