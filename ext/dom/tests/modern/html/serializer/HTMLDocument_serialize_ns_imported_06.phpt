--TEST--
DOM\HTMLDocument serialization with an imported namespace node 06
--EXTENSIONS--
dom
--FILE--
<?php

$xml = DOM\XMLDocument::createFromFile(__DIR__.'/sample.xml');
$xml->documentElement->firstChild->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXML();

echo "--- After clone + import into HTML ---\n";

$html = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->adoptNode($xml->documentElement->firstElementChild->cloneNode(true)));

echo $html->saveXML();
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
</container>
--- After clone + import into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head/><body><p>foo<x xmlns="some:ns">
        <subcontainer>
            <test xmlns="x:y"/>
            <child2/>
        </subcontainer>
        <subcontainer2>
            <foo xmlns="some:ns"/>
        </subcontainer2>
    </x></p></body></html>
<html><head></head><body><p>foo<x>
        <subcontainer>
            <test xmlns="x:y"></test>
            <child2></child2>
        </subcontainer>
        <subcontainer2>
            <foo xmlns="some:ns"></foo>
        </subcontainer2>
    </x></p></body></html>
