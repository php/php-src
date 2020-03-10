--TEST--
Bug #40451 (addAttribute() may crash when used with non-existent child node)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
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
Fatal error: Uncaught Error: Call to a member function addAttribute() on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
