--TEST--
Bug #45901 (wddx_serialize_value crash with SimpleXMLElement object)
--SKIPIF--
<?php
if (!extension_loaded("wddx")) print "skip";
if (!extension_loaded("simplexml")) print "skip SimpleXML not present";
?>
--FILE--
<?php

$xml = new SimpleXMLElement('<data></data>');
$xml->addChild('test');
echo wddx_serialize_value($xml, 'Variables') . "\n";
echo "DONE";
?>
--EXPECTF--
<wddxPacket version='1.0'><header><comment>Variables</comment></header><data><struct><var name='php_class_name'><string>SimpleXMLElement</string></var><var name='test'><struct><var name='php_class_name'><string>SimpleXMLElement</string></var></struct></var></struct></data></wddxPacket>
DONE