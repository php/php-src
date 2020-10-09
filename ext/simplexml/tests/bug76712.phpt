--TEST--
BUg #76712 (Assignment of empty string creates extraneous text node)
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml not available');
?>
--FILE--
<?php
$sxe = new SimpleXMLElement('<foo></foo>');
$sxe->addChild('bar', '');
echo $sxe->asXML();

$sxe = new SimpleXMLElement('<foo></foo>');
$sxe->addChild('bar');
$sxe->bar = '';
echo $sxe->asXML();
?>
--EXPECT--
<?xml version="1.0"?>
<foo><bar/></foo>
<?xml version="1.0"?>
<foo><bar/></foo>
