--TEST--
DOMDocument::createAttributeNS() with prefix name conflict - setAttributeNodeNS variation, with prefix
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));

var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns1', 'foo:hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns2', 'foo:hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns3', 'foo:hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns4', 'foo:hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

?>
--EXPECT--
NULL
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" foo:hello=""/>

NULL
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" xmlns:default="http://php.net/ns2" foo:hello="" default:hello=""/>

NULL
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" xmlns:default="http://php.net/ns2" xmlns:default1="http://php.net/ns3" foo:hello="" default:hello="" default1:hello=""/>

NULL
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" xmlns:default="http://php.net/ns2" xmlns:default1="http://php.net/ns3" xmlns:default2="http://php.net/ns4" foo:hello="" default:hello="" default1:hello="" default2:hello=""/>
