--TEST--
DOMNode::normalize() edge case
--EXTENSIONS--
dom
--FILE--
<?php

$doc = DOM\XMLDocument::createEmpty();
$container = $doc->appendChild($doc->createElement("container"));

$container->appendChild($doc->createTextNode(""));
$middle = $container->appendChild($doc->createTextNode("foo"));
$container->appendChild($doc->createTextNode(""));

$doc->normalizeDocument();

echo $doc->saveXML();

var_dump($middle->textContent);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<container>foo</container>
string(3) "foo"
