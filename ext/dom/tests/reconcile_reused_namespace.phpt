--TEST--
Reconcile a reused namespace from doc->oldNs
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$root = $dom->createElementNS('http://www.w3.org/2000/xhtml', 'html');

$dom->loadXML(<<<XML
<?xml version="1.0"?>
<html
    xmlns="http://www.w3.org/2000/xhtml"
    xmlns:a="http://example.com/A"
    xmlns:b="http://example.com/B"
/>
XML);
$root = $dom->firstElementChild;

echo "Add first\n";
$element = $dom->createElementNS('http://example.com/B', 'p', 'Hello World');
$root->appendChild($element);

echo "Add second\n";
$element = $dom->createElementNS('http://example.com/A', 'p', 'Hello World');
$root->appendChild($element);

echo "Add third\n";
$element = $dom->createElementNS('http://example.com/A', 'p', 'Hello World');
$root->appendChild($element);

var_dump($dom->saveXML());

?>
--EXPECT--
Add first
Add second
Add third
string(201) "<?xml version="1.0"?>
<html xmlns="http://www.w3.org/2000/xhtml" xmlns:a="http://example.com/A" xmlns:b="http://example.com/B"><b:p>Hello World</b:p><a:p>Hello World</a:p><a:p>Hello World</a:p></html>
"
