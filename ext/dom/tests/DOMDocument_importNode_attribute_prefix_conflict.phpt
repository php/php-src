--TEST--
DOMDocument::importNode() with attribute prefix name conflict
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Non-default namespace test case without a default namespace in the destination ---\n";

$dom1 = new DOMDocument();
$dom2 = new DOMDocument();
$dom1->loadXML('<?xml version="1.0"?><container xmlns:foo="http://php.net" foo:bar="yes"/>');
$dom2->loadXML('<?xml version="1.0"?><container xmlns:foo="http://php.net/2"/>');
$attribute = $dom1->documentElement->getAttributeNode('foo:bar');
$imported = $dom2->importNode($attribute);
$dom2->documentElement->setAttributeNodeNS($imported);

echo $dom1->saveXML();
echo $dom2->saveXML();

echo "--- Non-default namespace test case with a default namespace in the destination ---\n";

$dom1 = new DOMDocument();
$dom2 = new DOMDocument();
$dom1->loadXML('<?xml version="1.0"?><container xmlns:foo="http://php.net" foo:bar="yes"/>');
$dom2->loadXML('<?xml version="1.0"?><container xmlns="http://php.net" xmlns:foo="http://php.net/2"/>');
$attribute = $dom1->documentElement->getAttributeNode('foo:bar');
$imported = $dom2->importNode($attribute);
var_dump($imported->prefix, $imported->namespaceURI);
$dom2->documentElement->setAttributeNodeNS($imported);
var_dump($imported->prefix, $imported->namespaceURI);

echo $dom1->saveXML();
echo $dom2->saveXML();

echo "--- Default namespace test case ---\n";

// We don't expect the namespace to be imported because default namespaces on the same element don't apply to attributes
// but the attribute should be imported
$dom1 = new DOMDocument();
$dom2 = new DOMDocument();
$dom1->loadXML('<?xml version="1.0"?><container xmlns="http://php.net" bar="yes"/>');
$dom2->loadXML('<?xml version="1.0"?><container xmlns="http://php.net/2"/>');
$attribute = $dom1->documentElement->getAttributeNode('bar');
$imported = $dom2->importNode($attribute);
$dom2->documentElement->setAttributeNodeNS($imported);

echo $dom1->saveXML();
echo $dom2->saveXML();

?>
--EXPECT--
--- Non-default namespace test case without a default namespace in the destination ---
<?xml version="1.0"?>
<container xmlns:foo="http://php.net" foo:bar="yes"/>
<?xml version="1.0"?>
<container xmlns:foo="http://php.net/2" xmlns:default="http://php.net" default:bar="yes"/>
--- Non-default namespace test case with a default namespace in the destination ---
string(7) "default"
string(14) "http://php.net"
string(7) "default"
string(14) "http://php.net"
<?xml version="1.0"?>
<container xmlns:foo="http://php.net" foo:bar="yes"/>
<?xml version="1.0"?>
<container xmlns="http://php.net" xmlns:foo="http://php.net/2" xmlns:default="http://php.net" default:bar="yes"/>
--- Default namespace test case ---
<?xml version="1.0"?>
<container xmlns="http://php.net" bar="yes"/>
<?xml version="1.0"?>
<container xmlns="http://php.net/2" bar="yes"/>
