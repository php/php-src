--TEST--
DOMDocument::createAttributeNS() with prefix name conflict
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));

echo "--- setAttributeNode variation (you shouldn't do this, but we test this anyway) ---\n";

// Adheres to DOM Core Level 3

var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns1', 'hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns2', 'hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns3', 'hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns4', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns1', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns2', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns3', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns4', 'foo:hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

$doc = new DOMDocument();
$doc->appendChild($doc->createElement('container'));
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns1', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNode($doc->createAttributeNS('http://php.net/ns1', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

echo "--- setAttributeNodeNS variation ---\n";

var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns1', 'hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns2', 'hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns3', 'hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns4', 'hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns1', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns2', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns3', 'foo:hello'))?->namespaceURI);
var_dump($doc->documentElement->setAttributeNodeNS($doc->createAttributeNS('http://php.net/ns4', 'foo:hello'))?->namespaceURI);
echo $doc->saveXML(), "\n";

?>
--EXPECT--
--- setAttributeNode variation (you shouldn't do this, but we test this anyway) ---
NULL
string(18) "http://php.net/ns1"
string(18) "http://php.net/ns2"
string(18) "http://php.net/ns3"
<?xml version="1.0"?>
<container xmlns="http://php.net/ns1" xmlns:default="http://php.net/ns2" xmlns:default1="http://php.net/ns3" xmlns:default2="http://php.net/ns4" default2:hello=""/>

string(18) "http://php.net/ns4"
string(18) "http://php.net/ns1"
string(18) "http://php.net/ns2"
string(18) "http://php.net/ns3"
<?xml version="1.0"?>
<container xmlns="http://php.net/ns1" xmlns:default="http://php.net/ns2" xmlns:default1="http://php.net/ns3" xmlns:default2="http://php.net/ns4" xmlns:foo="http://php.net/ns1" default2:hello=""/>

NULL
string(18) "http://php.net/ns1"
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" foo:hello=""/>

--- setAttributeNodeNS variation ---
string(18) "http://php.net/ns1"
NULL
NULL
NULL
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" xmlns="http://php.net/ns2" xmlns:default="http://php.net/ns3" xmlns:default1="http://php.net/ns4" foo:hello="" hello="" default:hello="" default1:hello=""/>

string(18) "http://php.net/ns1"
string(18) "http://php.net/ns2"
string(18) "http://php.net/ns3"
string(18) "http://php.net/ns4"
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/ns1" xmlns="http://php.net/ns2" xmlns:default="http://php.net/ns3" xmlns:default1="http://php.net/ns4" xmlns:default2="http://php.net/ns2" foo:hello="" default2:hello="" default:hello="" default1:hello=""/>
