--TEST--
Document::createElement()
--EXTENSIONS--
dom
--FILE--
<?php

function dumpElement(DOM\Element $element) {
    echo "tagName: ", var_dump($element->tagName);
    echo "nodeName: ", var_dump($element->nodeName);
    echo "textContent: ", var_dump($element->textContent);
    echo "prefix: ", var_dump($element->prefix);
    echo "namespaceURI: ", var_dump($element->namespaceURI);
    echo $element->ownerDocument->saveHTML($element), "\n\n";
}

echo "--- Into rootless document ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$element = $dom->createElement("HTML", "&hello");
dumpElement($element);

$element = $dom->createElement("HEad");
dumpElement($element);

echo "--- Into document with HTML root ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$element = $dom->createElement("HTML", "&hello");
$dom->appendChild($element);
$element = $dom->createElement("HEad");
dumpElement($element);

echo "--- Into document with non-HTML root ---\n";

$dom = DOM\HTMLDocument::createEmpty();
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
prefix: string(0) ""
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<html>&amp;hello</html>

tagName: string(4) "HEAD"
nodeName: string(4) "HEAD"
textContent: string(0) ""
prefix: string(0) ""
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<head></head>

--- Into document with HTML root ---
tagName: string(4) "HEAD"
nodeName: string(4) "HEAD"
textContent: string(0) ""
prefix: string(0) ""
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<head></head>

--- Into document with non-HTML root ---
tagName: string(4) "HEAD"
nodeName: string(4) "HEAD"
textContent: string(0) ""
prefix: string(0) ""
namespaceURI: string(28) "http://www.w3.org/1999/xhtml"
<head></head>
