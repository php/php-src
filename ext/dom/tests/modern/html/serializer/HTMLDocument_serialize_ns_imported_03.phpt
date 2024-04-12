--TEST--
DOM\HTMLDocument serialization with an imported namespace node 03
--EXTENSIONS--
dom
--FILE--
<?php

$xml = DOM\XMLDocument::createFromFile(__DIR__.'/sample.xml');
$xml->documentElement->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXML(), "\n";

echo "--- After import into HTML ---\n";

$html = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->importNode($xml->documentElement, false));

echo $html->saveXML(), "\n";
echo $html->saveHTML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<container xmlns="some:ns" xmlns:bar="another:ns">
    <x>
        <subcontainer>
            <test xmlns="x:y"/>
            <child2/>
        </subcontainer>
        <subcontainer2>
            <foo xmlns="some:ns"/>
        </subcontainer2>
    </x>
<child xmlns="some:ns2"/></container>
--- After import into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"/></p></body></html>
<html><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns"></container></p></body></html>
