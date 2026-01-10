--TEST--
DOMElement::{get,has}AttributeNS() with xmlns
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<root xmlns:a="urn:a"/>');

var_dump($dom->documentElement->getAttributeNS('http://www.w3.org/2000/xmlns/', 'a'));
var_dump($dom->documentElement->getAttributeNS('http://www.w3.org/2000/xmlns/', 'b'));
var_dump($dom->documentElement->hasAttributeNS('http://www.w3.org/2000/xmlns/', 'a'));
var_dump($dom->documentElement->hasAttributeNS('http://www.w3.org/2000/xmlns/', 'b'));

?>
--EXPECT--
string(5) "urn:a"
string(0) ""
bool(true)
bool(false)
