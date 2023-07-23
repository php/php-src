--TEST--
setAttributeNodeNS with same URI but different prefix
--EXTENSIONS--
dom
--FILE--
<?php
// Spec doesn't require to remove redundant namespace declarations.
// This means that when appending a new attribute the old namespace declaration is reused, hence the prefix stays "foo".
// Firefox cleans up the old namespace declaration, so there the prefix does change.
// Chrome doesn't clean it up, so there the prefix doesn't change.
// Our behaviour is the same as Chrome's due to implementation difficulties.
$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));
$attribute = $doc->createAttributeNS('http://php.net/ns1', 'foo:hello');
$attribute->nodeValue = '1';
var_dump($doc->documentElement->setAttributeNodeNS($attribute)?->nodeValue);
echo $doc->saveXML(), "\n";
$attribute = $doc->createAttributeNS('http://php.net/ns1', 'bar:hello');
$attribute->nodeValue = '2';
var_dump($doc->documentElement->setAttributeNodeNS($attribute)?->nodeValue);
echo $doc->saveXML(), "\n";
$attribute = $doc->createAttributeNS('http://php.net/ns1', 'hello');
$attribute->nodeValue = '3';
var_dump($doc->documentElement->setAttributeNodeNS($attribute)?->nodeValue);
echo $doc->saveXML(), "\n";
?>
--EXPECT--
NULL
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" foo:hello="1"/>

string(1) "1"
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" foo:hello="2"/>

string(1) "2"
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" foo:hello="3"/>
