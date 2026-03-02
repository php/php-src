--TEST--
Test reading Element::$outerHTML on HTML documents
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);

$p = $dom->body->firstChild;
var_dump($p->outerHTML);

$root = $dom->documentElement;
var_dump($root->outerHTML);

$unattached_element = $dom->createElement('unattached');
var_dump($unattached_element->outerHTML);

$template = $dom->createElement('template');
$template->innerHTML = '<p>foo</p>';
var_dump($template->outerHTML);

?>
--EXPECT--
string(10) "<p>foo</p>"
string(49) "<html><head></head><body><p>foo</p></body></html>"
string(25) "<unattached></unattached>"
string(31) "<template><p>foo</p></template>"
