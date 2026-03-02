--TEST--
Lifetime issue with parentNode on getAttributeNodeNS()
--EXTENSIONS--
dom
--FILE--
<?php
$xmlString = '<?xml version="1.0" encoding="utf-8" ?>
<root xmlns="http://ns" xmlns:ns2="http://ns2">
    <ns2:child />
</root>';

$xml=new DOMDocument();
$xml->loadXML($xmlString);
$ns2 = $xml->documentElement->getAttributeNodeNS("http://www.w3.org/2000/xmlns/", "ns2");
$ns2->parentNode->remove();
var_dump($ns2->parentNode->localName);

?>
--EXPECT--
string(4) "root"
