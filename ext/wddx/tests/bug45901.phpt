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

Warning: wddx_serialize_value(): Class SimpleXMLElement can not be serialized in %sbug45901.php on line %d
<wddxPacket version='1.0'><header><comment>Variables</comment></header><data></data></wddxPacket>
DONE