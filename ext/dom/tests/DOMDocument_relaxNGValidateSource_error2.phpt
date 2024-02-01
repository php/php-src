--TEST--
DOMDocument::relaxNGValidateSource() should fail on invalid RNG schema
--CREDITS--
Knut Urdalen <knut@php.net>
--EXTENSIONS--
dom
--FILE--
<?php
$rng = <<< RNG
<?xml version="1.0" encoding="UTF-8"?>
<grammar ns="" xmlns="http://relaxng.org/ns/structure/1.0"
  datatypeLibrary="http://www.w3.org/2001/XMLSchema-datatypes">
  <start>
    <element name="apple">
    </element>
  </start>
</grammar>
RNG;

$xml = <<< XML
<?xml version="1.0"?>
<apple>
  <pear>Pear</pear>
</apple>
XML;

$doc = new DOMDocument();
$doc->loadXML($xml);
$result = $doc->relaxNGValidateSource($rng);
var_dump($result);

?>
--EXPECTF--
Warning: DOM\Document::relaxNGValidateSource(): xmlRelaxNGParseElement: element has no content in %s on line %d

Warning: DOM\Document::relaxNGValidateSource(): Invalid RelaxNG in %s on line %d
bool(false)
