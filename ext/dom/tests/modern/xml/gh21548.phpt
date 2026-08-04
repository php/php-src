--TEST--
GH-21548 (Dom\XMLDocument::C14N() emits duplicate xmlns declarations after setAttributeNS())
--CREDITS--
Toon Verwerft (veewee)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = Dom\XMLDocument::createFromString('<root xmlns="urn:a" attr="val"/>');
$doc->documentElement->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns1", "urn:a");

echo $doc->C14N() . PHP_EOL;

?>
--EXPECT--
<root xmlns="urn:a" xmlns:ns1="urn:a" attr="val"></root>
