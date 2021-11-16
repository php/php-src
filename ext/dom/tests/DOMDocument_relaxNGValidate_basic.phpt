--TEST--
DOMDocument::relaxNGValidate()
--CREDITS--
Knut Urdalen <knut@php.net>
--EXTENSIONS--
dom
--FILE--
<?php
$rng = __DIR__.'/DOMDocument_relaxNGValidate_basic.rng';
$xml = <<< XML
<?xml version="1.0"?>
<apple>
  <pear>Pear</pear>
</apple>
XML;
$doc = new DOMDocument();
$doc->loadXML($xml);
$result = $doc->relaxNGValidate($rng);
var_dump($result);
?>
--EXPECT--
bool(true)
