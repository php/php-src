--TEST--
SimpleXMLElement: Test to ensure that the required attribute name correctly is giving a warning
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php if (!extension_loaded("simplexml")) { echo "skip extension not available"; } ?>
--FILE--
<?php
$a = new SimpleXMLElement("<php>testfest</php>");
$a->addAttribute( "", "" );
echo $a->asXML();
?>
--EXPECTF--
Warning: SimpleXMLElement::addAttribute(): Attribute name is required in %s on line %d
<?xml version="1.0"?>
<php>testfest</php>
