--TEST--
Bug #66937 (transformToDoc() removes DTD of source document)
--SKIPIF--
<?php
if (!extension_loaded('xsl')) die('skip xsl extension not loaded');
?>
--FILE--
<?php
$stylesheet = new DOMDocument;
$stylesheet->loadXML('<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"/>');

$processor = new XSLTProcessor;
$processor->importStylesheet($stylesheet);

$doc = new DOMDocument;
$doc->loadXML('<?xml version="1.0"?>
<!DOCTYPE article PUBLIC "-//example//EN" "http://example.org/dtd">
<article/>');

printf("Doctype ID: %s\n", $doc->doctype->publicId);
printf("Next sibling: %s\n", $doc->doctype->nextSibling->nodeName);
echo $doc->saveXML();

$output = $processor->transformToDoc($doc);

printf("\nDoctype ID: %s\n", $doc->doctype->publicId);
printf("Next sibling: %s\n", $doc->doctype->nextSibling->nodeName);
echo $doc->saveXML();
?>
--EXPECT--
Doctype ID: -//example//EN
Next sibling: article
<?xml version="1.0"?>
<!DOCTYPE article PUBLIC "-//example//EN" "http://example.org/dtd">
<article/>

Doctype ID: -//example//EN
Next sibling: article
<?xml version="1.0"?>
<!DOCTYPE article PUBLIC "-//example//EN" "http://example.org/dtd">
<article/>
