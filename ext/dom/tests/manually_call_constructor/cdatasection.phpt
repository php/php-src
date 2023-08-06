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
--EXPECTF--
string(8) "my value"

Fatal error: Uncaught Error: Couldn't fetch DOMCdataSection. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
