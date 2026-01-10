--TEST--
DOMDocument::createAttributeNS() with prefix name conflict - setAttributeNodeNS variation, without prefix
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));

var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns1', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns2', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns3', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns4', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

?>
--EXPECT--
NULL
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" default:hello=""/>

NULL
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" xmlns:default1="http://php.net/ns2" default:hello="" default1:hello=""/>

NULL
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" xmlns:default1="http://php.net/ns2" xmlns:default2="http://php.net/ns3" default:hello="" default1:hello="" default2:hello=""/>

NULL
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" xmlns:default1="http://php.net/ns2" xmlns:default2="http://php.net/ns3" xmlns:default3="http://php.net/ns4" default:hello="" default1:hello="" default2:hello="" default3:hello=""/>
