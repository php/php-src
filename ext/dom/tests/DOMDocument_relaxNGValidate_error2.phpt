--TEST--
DOMDocument::relaxNGValidate() should fail on invalid RelaxNG file source
--CREDITS--
Knut Urdalen <knut@php.net>
--EXTENSIONS--
dom
--FILE--
<?php
$rng = __DIR__.'/foo.rng';
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
Warning: DOM\Document::relaxNGValidate(): I/O warning : failed to load external entity "/home/niels/php-src/ext/dom/tests/foo.rng" in %s on line %d

Warning: DOM\Document::relaxNGValidate(): xmlRelaxNGParse: could not load /home/niels/php-src/ext/dom/tests/foo.rng in %s on line %d

Warning: DOM\Document::relaxNGValidate(): Invalid RelaxNG in %s on line %d
bool(false)
