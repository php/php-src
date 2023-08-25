--TEST--
Document serialization with an imported namespace node 05
--EXTENSIONS--
dom
--FILE--
<?php

$xml = new DOM\HTML5Document();
$xml->loadXML('<?xml version="1.0"?><container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/></container>');
$xml->documentElement->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXML();
echo $xml->saveHTML(), "\n";

echo "--- After adoption into HTML ---\n";

$html = new DOM\HTML5Document();
$html->loadHTML('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->adoptNode($xml->documentElement));

echo $html->saveXML();
echo $html->saveHTML(), "\n";

?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/><child xmlns="some:ns2"/></container>
<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"></test><child></child></container>
--- After adoption into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head/><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/><child xmlns="some:ns2"/></container></p></body></html>
<html><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"></test><child></child></container></p></body></html>
