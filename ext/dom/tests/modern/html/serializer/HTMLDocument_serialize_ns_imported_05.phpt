--TEST--
Dom\HTMLDocument serialization with an imported namespace node 05
--EXTENSIONS--
dom
--FILE--
<?php

$xml = Dom\XMLDocument::createFromFile(__DIR__.'/sample.xml');
$xml->documentElement->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXml(), "\n";

echo "--- After adoption into HTML ---\n";

$html = Dom\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->adoptNode($xml->documentElement));

echo $html->saveXml(), "\n";
echo $html->saveHtml(), "\n";

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
--- After adoption into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns">
    <x>
        <subcontainer>
            <test xmlns="x:y"/>
            <child2/>
        </subcontainer>
        <subcontainer2>
            <foo xmlns="some:ns"/>
        </subcontainer2>
    </x>
<child xmlns="some:ns2"/></container></p></body></html>
<html><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns">
    <x>
        <subcontainer>
            <test xmlns="x:y"></test>
            <child2></child2>
        </subcontainer>
        <subcontainer2>
            <foo xmlns="some:ns"></foo>
        </subcontainer2>
    </x>
<child></child></container></p></body></html>
