--TEST--
XML parsing with LIBXML_RECOVER
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<root>foo</root/>', options: LIBXML_RECOVER);
echo $dom->saveXML();

$dom = DOM\XMLDocument::createFromString('<root>foo</root/>', options: LIBXML_RECOVER);
echo $dom->saveXML(), "\n";

?>
--EXPECTF--
Warning: DOMDocument::loadXML(): expected '>' in Entity, line: 1 in %s on line %d
<?xml version="1.0"?>
<root>foo</root>

Warning: DOM\XMLDocument::createFromString(): expected '>' in Entity, line: 1 in %s on line %d
<?xml version="1.0" encoding="UTF-8"?>
<root>foo</root>
