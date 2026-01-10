--TEST--
Test DOM\Document::$body setter
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Replace body with itself ---\n";
$dom = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);
$dom->body = $dom->body;
var_dump($dom->body?->nodeName);

echo "--- Add body when there is no body yet ---\n";
$dom = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);
$dom->body->remove();
$dom->body = $dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix:body");
var_dump($dom->body?->nodeName);

echo "--- Replace old body with new body ---\n";
$dom = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);
$dom->body = $dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix:body");
var_dump($dom->body?->nodeName);

echo "--- Replace old body with new body, while still having a reference to the old body ---\n";
$dom = DOM\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);
$old = $dom->body;
$dom->body = $dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix:body");
var_dump($dom->body?->nodeName);
var_dump($old->nodeName);

echo "--- Special note from the DOM spec ---\n";
$dom = DOM\XMLDocument::createFromString('<svg xmlns="http://www.w3.org/2000/svg"/>');
$dom->body = $dom->createElementNS("http://www.w3.org/1999/xhtml", "body");
var_dump($dom->body?->nodeName);

?>
--EXPECT--
--- Replace body with itself ---
string(4) "BODY"
--- Add body when there is no body yet ---
string(11) "PREFIX:BODY"
--- Replace old body with new body ---
string(11) "PREFIX:BODY"
--- Replace old body with new body, while still having a reference to the old body ---
string(11) "PREFIX:BODY"
string(4) "BODY"
--- Special note from the DOM spec ---
NULL
