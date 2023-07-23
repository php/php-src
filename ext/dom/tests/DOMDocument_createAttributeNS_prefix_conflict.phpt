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
--EXPECTF--
--- setAttributeNode variation (you shouldn't do this, but we test this anyway) ---
NULL

Fatal error: Uncaught DOMException: Namespace Error in %s:%d
Stack trace:
#0 %s(%d): DOMDocument->createAttributeNS('http://php.net/...', 'hello')
#1 {main}
  thrown in %s on line %d
