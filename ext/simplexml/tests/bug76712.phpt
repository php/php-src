--TEST--
BUg #76712 (Assignment of empty string creates extraneous text node)
--EXTENSIONS--
simplexml
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
