--TEST--
Document serialization with an imported namespace node 03
--EXTENSIONS--
dom
--FILE--
<?php

$xml = new DOM\HTML5Document();
$xml->loadXML('<?xml version="1.0"?><container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/></container>');
$xml->documentElement->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXML();
echo $xml->saveHTML(), "\n";

echo "--- After import into HTML ---\n";

$html = new DOM\HTML5Document();
$html->loadHTML('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->importNode($xml->documentElement, false));

echo $html->saveXML();
echo $html->saveHTML(), "\n";

?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/><child xmlns="some:ns2"/></container>
<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"></test><child></child></container>
--- After import into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head/><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"/></p></body></html>
<html><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"></container></p></body></html>
