--TEST--
GH-19612 (Mitigate libxml2 tree dictionary bug)
--EXTENSIONS--
dom
--FILE--
<?php
$xml = new DOMDocument;
$xml->loadXML(<<<XML
<!DOCTYPE root [
<!ENTITY foo "foo">
]>
<root><el x="&foo;"/></root>
XML);
$html = new DOMDocument;
$html->loadHTML('<p>foo</p>', LIBXML_NOERROR);
$p = $html->documentElement->firstChild->firstChild;
$p->appendChild($html->adoptNode($xml->documentElement->firstElementChild->cloneNode(true)));

echo $html->saveXML();
echo $xml->saveXML();
?>
--EXPECT--
<?xml version="1.0" standalone="yes"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html><body><p>foo<el x="&foo;"/></p></body></html>
<?xml version="1.0"?>
<!DOCTYPE root [
<!ENTITY foo "foo">
]>
<root><el x="&foo;"/></root>
