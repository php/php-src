--TEST--
DOMDocument::createAttributeNS() with prefix name conflict - setAttributeNode variation (DOM Level 3), without prefix
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));

var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns1', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns2', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns3', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns4', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

?>
--EXPECT--
NULL
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" default:hello=""/>

string(18) "http://php.net/ns1"
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" xmlns:default1="http://php.net/ns2" default1:hello=""/>

string(18) "http://php.net/ns2"
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" xmlns:default1="http://php.net/ns2" xmlns:default2="http://php.net/ns3" default2:hello=""/>

string(18) "http://php.net/ns3"
<?xml version="1.0"?>
<container xmlns:default="http://php.net/ns1" xmlns:default1="http://php.net/ns2" xmlns:default2="http://php.net/ns3" xmlns:default3="http://php.net/ns4" default3:hello=""/>
