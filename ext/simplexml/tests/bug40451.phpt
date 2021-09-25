--TEST--
Bug #40451 (addAttribute() may crash when used with non-existent child node)
--EXTENSIONS--
simplexml
--FILE--
<?php

$string = <<<XML
<?xml version="1.0"?>
    <Host enable="true">
     <Name>host.server.com</Name>
     </Host>
XML;

$xml = simplexml_load_string($string);

$add = $xml->addChild('Host');
$add->Host->addAttribute('enable', 'true');

?>
--EXPECTF--
Warning: SimpleXMLElement::addAttribute(): Unable to locate parent Element in %s on line %d
