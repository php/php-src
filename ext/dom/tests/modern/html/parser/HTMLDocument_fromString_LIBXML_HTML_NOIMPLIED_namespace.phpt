--TEST--
Dom\HTMLDocument::createFromString() with LIBXML_HTML_NOIMPLIED namespace check
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- No elements ---\n";

$dom = Dom\HTMLDocument::createFromString("", LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
echo $dom->saveXml(), "\n";

echo "--- Single element ---\n";

$dom = Dom\HTMLDocument::createFromString("<p>foo</p>", LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
echo $dom->saveXml(), "\n";
var_dump($dom->documentElement->namespaceURI);
var_dump($dom->documentElement->prefix);

echo "--- Multiple elements ---\n";

$dom = Dom\HTMLDocument::createFromString("<p>foo</p><strong>bar</strong>", LIBXML_HTML_NOIMPLIED | LIBXML_NOERROR);
echo $dom->saveXml(), "\n";
var_dump($dom->documentElement->namespaceURI);
var_dump($dom->documentElement->prefix);
var_dump($dom->documentElement->nextSibling->namespaceURI);
var_dump($dom->documentElement->nextSibling->prefix);

?>
--EXPECT--
--- No elements ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>

--- Single element ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<p xmlns="http://www.w3.org/1999/xhtml">foo</p>
string(28) "http://www.w3.org/1999/xhtml"
NULL
--- Multiple elements ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<p xmlns="http://www.w3.org/1999/xhtml">foo</p><strong xmlns="http://www.w3.org/1999/xhtml">bar</strong>
string(28) "http://www.w3.org/1999/xhtml"
NULL
string(28) "http://www.w3.org/1999/xhtml"
NULL
