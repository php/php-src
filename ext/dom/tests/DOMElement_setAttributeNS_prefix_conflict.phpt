--TEST--
DOMElement::setAttributeNS() with prefix name conflict
--EXTENSIONS--
dom
--FILE--
<?php
echo "--- Non-default namespace test case ---\n";

$dom = new DOMDocument();
$dom->loadXML('<?xml version="1.0"?><container xmlns:foo="http://php.net" foo:bar="yes"/>');
$dom->documentElement->setAttributeNS('http://php.net/2', 'foo:bar', 'no1');
echo $dom->saveXML();
$dom->documentElement->setAttributeNS('http://php.net/2', 'bar', 'no2');
echo $dom->saveXML();

echo "--- Default namespace test case ---\n";

$dom = new DOMDocument();
$dom->loadXML('<?xml version="1.0"?><container xmlns="http://php.net" bar="yes"/>');
$dom->documentElement->setAttributeNS('http://php.net/2', 'bar', 'no1');
echo $dom->saveXML();
$dom->documentElement->setAttributeNS('http://php.net/2', 'bar', 'no2');
echo $dom->saveXML();
?>
--EXPECT--
--- Non-default namespace test case ---
<?xml version="1.0"?>
<container xmlns:foo="http://php.net" xmlns:default="http://php.net/2" foo:bar="yes" default:bar="no1"/>
<?xml version="1.0"?>
<container xmlns:foo="http://php.net" xmlns:default="http://php.net/2" foo:bar="yes" default:bar="no2"/>
--- Default namespace test case ---
<?xml version="1.0"?>
<container xmlns="http://php.net" xmlns:default="http://php.net/2" bar="yes" default:bar="no1"/>
<?xml version="1.0"?>
<container xmlns="http://php.net" xmlns:default="http://php.net/2" bar="yes" default:bar="no2"/>
