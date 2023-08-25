--TEST--
Document serialization with an imported namespace node 06
--EXTENSIONS--
dom
--FILE--
<?php

$xml = new DOM\HTML5Document();
$xml->loadXML('<?xml version="1.0"?><container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"/><child2/></container>');
$xml->documentElement->firstChild->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXML();
echo $xml->saveHTML(), "\n";

echo "--- After clone + import into HTML ---\n";

$html = new DOM\HTML5Document();
$html->loadHTML('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->adoptNode($xml->documentElement->firstChild->cloneNode(true)));

echo $html->saveXML();
echo $html->saveHTML(), "\n";

?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"><child xmlns="some:ns2"/></test><child2/></container>
<container xmlns="some:ns" xmlns:bar="another:ns"><test xmlns="x:y"><child></child></test><child2></child2></container>
--- After clone + import into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head/><body><p>foo<test xmlns="x:y"><child xmlns="some:ns2"/></test></p></body></html>
<html><head></head><body><p>foo<test xmlns="x:y"><child></child></test></p></body></html>
