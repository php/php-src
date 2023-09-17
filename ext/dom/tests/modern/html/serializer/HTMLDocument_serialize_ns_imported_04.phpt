--TEST--
DOM\HTMLDocument serialization with an imported namespace node 04
--EXTENSIONS--
dom
--FILE--
<?php

$xml = DOM\XMLDocument::createFromString('<?xml version="1.0"?><container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/></container>');
$xml->documentElement->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXML();

echo "--- After import into HTML ---\n";

$html = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->importNode($xml->documentElement, false));

echo $html->saveXML();
echo $html->saveHTML(), "\n";

?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/><child xmlns="some:ns2"/></container>
--- After import into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head/><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"/></p></body></html>
<html><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"></container></p></body></html>
