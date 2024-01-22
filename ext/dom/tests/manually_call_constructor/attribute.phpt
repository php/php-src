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
--EXPECT--
string(9) "attribute"
string(8) "my value"
string(12) "newattribute"
string(12) "my new value"
<?xml version="1.0"?>
<container newattribute="my new value"/>
<?xml version="1.0"?>
<container newattribute="my new value" newnewattribute="my even newer value"/>
