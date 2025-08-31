--TEST--
DOM: specific namespace behaviour for applications with fixed serialization requirements
--EXTENSIONS--
dom
--FILE--
<?php

$dom1 = new DOMDocument();
$dom1->loadXML(<<<XML
<wsse:Security xmlns:wsse="foo:bar">
    <ds:Signature xmlns:ds="http://www.w3.org/2000/09/xmldsig#">
    </ds:Signature>
</wsse:Security>
XML);
$dom2 = new DOMDocument();
$dom2->loadXML('<xml><child/></xml>');
$wsse = $dom2->importNode($dom1->documentElement, true);
$dom2->firstChild->firstChild->appendChild($wsse);
echo $dom2->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<xml><child><wsse:Security xmlns:wsse="foo:bar" xmlns:ds="http://www.w3.org/2000/09/xmldsig#">
    <ds:Signature xmlns:ds="http://www.w3.org/2000/09/xmldsig#">
    </ds:Signature>
</wsse:Security></child></xml>
