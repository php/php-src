--TEST--
Manually call __construct() - attribute variation
--EXTENSIONS--
dom
--FILE--
<?php

$attr = new DOMAttr("attribute", "my value");
var_dump($attr->nodeName, $attr->nodeValue);
$attr->__construct("newattribute", "my new value");
var_dump($attr->nodeName, $attr->nodeValue);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
$doc->documentElement->setAttributeNode($attr);
echo $doc->saveXML();

$attr->__construct("newnewattribute", "my even newer value");
$doc->documentElement->setAttributeNode($attr);
echo $doc->saveXML();

?>
--EXPECTF--
string(9) "attribute"
string(8) "my value"

Fatal error: Uncaught Error: Couldn't fetch DOMAttr. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
