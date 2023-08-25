--TEST--
DOM\HTML5Document::loadHTML() with LIBXML_HTML_NOIMPLIED namespace check
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML("<p>foo</p>", LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
echo $dom->saveXML();
var_dump($dom->documentElement->namespaceURI);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<p xmlns="http://www.w3.org/1999/xhtml">foo</p>
string(28) "http://www.w3.org/1999/xhtml"
