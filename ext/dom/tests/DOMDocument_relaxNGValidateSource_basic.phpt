--TEST--
DOMDocument::relaxNGValidateSource()
--CREDITS--
Knut Urdalen <knut@php.net>
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$rng = <<< RNG
<?xml version="1.0" encoding="UTF-8"?>
<grammar ns="" xmlns="http://relaxng.org/ns/structure/1.0"
  datatypeLibrary="http://www.w3.org/2001/XMLSchema-datatypes">
  <start>
    <element name="apple">
      <element name="pear">
        <data type="NCName"/>
      </element>
    </element>
  </start>
</grammar>
RNG;

$good_xml = <<< GOOD_XML
<?xml version="1.0"?>
<apple>
  <pear>Pear</pear>
</apple>
GOOD_XML;

$doc = new DOMDocument();
$doc->loadXML($good_xml);
$result = $doc->relaxNGValidateSource($rng);
var_dump($result);

?>
--EXPECT--
bool(true)
