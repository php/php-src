--TEST--
DOMImplementation Tests
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$domimp = new DOMImplementation();
var_dump($domimp->hasFeature("1.0", "Core"));

$doctype = $domimp->createDocumentType("HTML", "-//W3C//DTD HTML 4.01//EN", "http://www.w3.org/TR/html4/strict.dtd");

$dom = $domimp->createDocument(NULL, "HTML", $doctype);

$html = $dom->documentElement;

$html->appendChild(new DOMElement("body", "hello world"));
$html->firstChild->appendChild(new DOMEntityReference("preformatted"));

print $dom->saveXML();
?>
--EXPECT--
bool(false)
<?xml version="1.0"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<HTML><body>hello world&preformatted;</body></HTML>
