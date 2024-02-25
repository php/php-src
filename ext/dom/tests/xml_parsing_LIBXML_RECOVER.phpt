--TEST--
XML parsing with LIBXML_RECOVER
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<root><child/>', options: LIBXML_RECOVER);
echo $dom->saveXML();

$dom = DOM\XMLDocument::createFromString('<root><child/>', options: LIBXML_RECOVER);
echo $dom->saveXML(), "\n";

?>
--EXPECTF--
Warning: DOMDocument::loadXML(): Premature end of data in tag root line 1 in Entity, line: 1 in %s on line %d
<?xml version="1.0"?>
<root><child/></root>

Warning: DOM\XMLDocument::createFromString(): Premature end of data in tag root line 1 in Entity, line: 1 in %s on line %d
<?xml version="1.0" encoding="UTF-8"?>
<root><child/></root>
