--TEST--
Manually call __construct() - CDATA section variation
--EXTENSIONS--
dom
--FILE--
<?php

$cdata = new DOMCdataSection("my value");
var_dump($cdata->nodeValue);
$cdata->__construct("my new value");
var_dump($cdata->nodeValue);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
$doc->documentElement->appendChild($cdata);
echo $doc->saveXML();

$cdata->__construct("my even newer value");
$doc->documentElement->appendChild($cdata);
echo $doc->saveXML();

?>
--EXPECT--
string(8) "my value"
string(12) "my new value"
<?xml version="1.0"?>
<container><![CDATA[my new value]]></container>
<?xml version="1.0"?>
<container><![CDATA[my new value]]><![CDATA[my even newer value]]></container>
