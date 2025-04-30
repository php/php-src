--TEST--
Document::createElement()
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/element_dump.inc";

echo "--- Into rootless document ---\n";

$dom = Dom\HTMLDocument::createEmpty();
$element = $dom->createElement("HTML");
$element->textContent = "&hello";
dumpElement($element);

$element = $dom->createElement("HEad");
dumpElement($element);

echo "--- Into document with HTML root ---\n";

$dom = Dom\HTMLDocument::createEmpty();
$element = $dom->createElement("HTML");
$element->textContent = "&hello";
$dom->appendChild($element);
$element = $dom->createElement("HEad");
dumpElement($element);

echo "--- Into document with non-HTML root ---\n";

$dom = Dom\HTMLDocument::createEmpty();
$element = $dom->createElementNS("urn:a", "container");
$dom->appendChild($element);
$element = $dom->createElement("HEad");
dumpElement($element);

?>
--EXPECT--
--- Into rootless document ---
tagName: string(4) "HTML"
nodeName: string(4) "HTML"
textContent: string(6) "&hello"
prefix: NULL
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<html>&amp;hello</html>

tagName: string(4) "HEAD"
nodeName: string(4) "HEAD"
textContent: string(0) ""
prefix: NULL
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<head></head>

--- Into document with HTML root ---
tagName: string(4) "HEAD"
nodeName: string(4) "HEAD"
textContent: string(0) ""
prefix: NULL
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<head></head>

--- Into document with non-HTML root ---
tagName: string(4) "HEAD"
nodeName: string(4) "HEAD"
textContent: string(0) ""
prefix: NULL
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<head></head>
