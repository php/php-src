--TEST--
GH-12616 (DOM: Removing XMLNS namespace node results in invalid default: prefix)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML(
    <<<XML
    <container xmlns:test="urn:test" xmlns:symfony="http://symfony.com/schema/dic/services">
        <symfony:services>
            <test:service id="hello" />
        </symfony:services>
    </container>
    XML
);

$doc->documentElement->removeAttributeNS('http://symfony.com/schema/dic/services', 'symfony');
$xpath = new DOMXPath($doc);
$xpath->registerNamespace('test', 'urn:test');

echo $doc->saveXML();

$result = $xpath->query('//container/services/test:service[@id="hello"]');
var_dump($result);

?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns:test="urn:test">
    <services>
        <test:service id="hello"/>
    </services>
</container>
object(DOMNodeList)#4 (1) {
  ["length"]=>
  int(1)
}
