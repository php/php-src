--TEST--
Test writing Element::$outerHTML on XML documents
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString("<root xmlns='urn:a'><x/></root>");
$dom->documentElement->firstChild->outerHTML = '<x xmlns="urn:a"><p>foo</p><p xmlns="urn:x">bar</p></x>';
echo $dom->saveXML(), "\n";

$dom->documentElement->firstChild->outerHTML = $dom->documentElement->firstChild->outerHTML;
$element = $dom->documentElement->firstChild->firstChild;
echo $dom->saveXML(), "\n";

$dom->documentElement->firstChild->outerHTML = 'tést';
echo $dom->saveXML(), "\n";

var_dump($element->tagName);

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement('p'));
$fragment->firstChild->outerHTML = '<strong>bar</strong>';
echo $dom->saveXML($fragment), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a"><x xmlns="urn:a"><p>foo</p><p xmlns="urn:x">bar</p></x></root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a"><x xmlns="urn:a"><p>foo</p><p xmlns="urn:x">bar</p></x></root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a">tést</root>
string(1) "p"
<strong xmlns="http://www.w3.org/1999/xhtml">bar</strong>
