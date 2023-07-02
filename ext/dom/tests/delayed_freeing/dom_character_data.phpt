--TEST--
Delayed freeing character data
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<'XML'
<?xml version="1.0"?>
<container><![CDATA[This is a CDATA section<p>test</p>]]></container>
XML);
$cdata = $doc->documentElement->firstChild;
var_dump($cdata->wholeText, $cdata->parentNode->tagName);
$cdata->parentNode->remove();
var_dump($cdata->wholeText, $cdata->parentNode->tagName);
?>
--EXPECTF--
string(34) "This is a CDATA section<p>test</p>"
string(9) "container"

Fatal error: Uncaught Error: Couldn't fetch DOMCdataSection. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
