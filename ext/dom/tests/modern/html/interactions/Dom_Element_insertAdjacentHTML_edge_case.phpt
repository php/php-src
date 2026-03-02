--TEST--
Dom\Element::insertAdjacentHTML() with HTML nodes - edge case
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString("", LIBXML_NOERROR);

$fragment = $dom->createDocumentFragment();
$node = $fragment->appendChild($dom->createElement("node"));

$node->insertAdjacentHTML(Dom\AdjacentPosition::BeforeBegin, "<p>foo</p>");

echo $dom->saveHtml($fragment), "\n";

$dom->firstChild->insertAdjacentHTML(Dom\AdjacentPosition::AfterBegin, $node->outerHTML);

echo $dom->saveHtml(), "\n";

?>
--EXPECT--
<p>foo</p><node></node>
<html><node></node><head></head><body></body></html>
