--TEST--
Bug #46185 (importNode changes the namespace of an XML element).
--EXTENSIONS--
dom
--FILE--
<?php
$aDOM = new DOMDocument();
$aDOM->loadXML('<?xml version="1.0"?>
<ns1:a xmlns:ns1="urn::ns"/>');
$a= $aDOM->firstChild;

$ok = new DOMDocument();
$ok->loadXML('<?xml version="1.0"?>
<ns1:ok xmlns:ns1="urn::ns" xmlns="urn::REAL"><watch-me xmlns:default="urn::BOGUS"/></ns1:ok>');

$imported= $aDOM->importNode($ok->firstChild, true);
$a->appendChild($imported);

echo $aDOM->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<ns1:a xmlns:ns1="urn::ns"><ns1:ok xmlns="urn::REAL"><watch-me xmlns:default="urn::BOGUS"/></ns1:ok></ns1:a>
