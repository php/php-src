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
--EXPECTF--
--- Non-default namespace test case ---

Fatal error: Uncaught DOMException: Namespace Error in %s:%d
Stack trace:
#0 %s(%d): DOMElement->setAttributeNS('http://php.net/...', 'foo:bar', 'no1')
#1 {main}
  thrown in %s on line %d
