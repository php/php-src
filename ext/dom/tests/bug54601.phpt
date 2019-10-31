--TEST--
Segfault when removing the Doctype node
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$xml = <<< XML
<?xml version='1.0' encoding='utf-8' ?>
<!DOCTYPE set PUBLIC "-//OASIS//DTD DocBook XML V5.0//EN" "http://www.docbook.org/xml/5.0/dtd/docbook.dtd" [
<!ENTITY foo '<foo>footext</foo>'>
<!ENTITY bar '<bar>bartext</bar>'>
]>
<set>&foo;&bar;</set>
XML;

$doc = new DOMDocument();
$doc->loadXML($xml, LIBXML_NOENT);
$n = $doc->doctype;
$doc->removeChild($n);
echo get_class($n), "\n";
print $doc->saveXML();
?>
--EXPECT--
DOMDocumentType
<?xml version="1.0" encoding="utf-8"?>
<set><foo>footext</foo><bar>bartext</bar></set>
