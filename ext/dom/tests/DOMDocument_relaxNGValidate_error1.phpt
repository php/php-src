--TEST--
DOMDocument::relaxNGValidate() should fail if document doesn't validate
--CREDITS--
Knut Urdalen <knut@php.net>
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$rng = __DIR__.'/DOMDocument_relaxNGValidate_basic.rng';
$xml = <<< XML
<?xml version="1.0"?>
<apple> 
  <pear>Pear</pear> 
  <pear>Pear</pear> 
</apple>
XML;
$doc = new DOMDocument();
$doc->loadXML($xml);
$result = $doc->relaxNGValidate($rng);
var_dump($result);
?>
--EXPECTF--
Warning: DOMDocument::relaxNGValidate(): Did not expect element pear there in %s on line %d
bool(false)
