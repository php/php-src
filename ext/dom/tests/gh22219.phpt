--TEST--
GH-22219 (Dom\XMLDocument::schemaValidate fails to resolve xs:QName value from an in-scope prefix)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (!method_exists('Dom\XMLDocument', 'schemaValidateSource')) die('skip schema validation not available');
?>
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:test" xmlns:ref="urn:other">
  <item target="ref:something"/>
</root>
XML;

// The 'ref' prefix is declared on <root> but only used inside the xs:QName
// attribute value, never as an element or attribute namespace.
$xsd = <<<XSD
<?xml version="1.0" encoding="UTF-8"?>
<schema xmlns="http://www.w3.org/2001/XMLSchema"
        targetNamespace="urn:test" elementFormDefault="qualified">
  <element name="root">
    <complexType>
      <sequence>
        <element name="item">
          <complexType>
            <attribute name="target" type="QName"/>
          </complexType>
        </element>
      </sequence>
    </complexType>
  </element>
</schema>
XSD;

libxml_use_internal_errors(true);

$modern = Dom\XMLDocument::createFromString($xml, LIBXML_NSCLEAN);
var_dump($modern->schemaValidateSource($xsd));

$legacy = new DOMDocument();
$legacy->loadXML($xml, LIBXML_NSCLEAN);
var_dump($legacy->schemaValidateSource($xsd));

foreach (libxml_get_errors() as $error) {
    echo trim($error->message), PHP_EOL;
}
?>
--EXPECT--
bool(true)
bool(true)
