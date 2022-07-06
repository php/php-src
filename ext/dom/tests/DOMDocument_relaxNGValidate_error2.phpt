--TEST--
DOMDocument::relaxNGValidate() should fail on invalid RelaxNG file source
--CREDITS--
Knut Urdalen <knut@php.net>
--SKIPIF--
<?php
require_once('skipif.inc');
?>
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
Warning: DOMDocument::relaxNGValidate(): I/O warning : failed to load external entity "%s/foo.rng" in %s on line %d

Warning: DOMDocument::relaxNGValidate(): xmlRelaxNGParse: could not load %s/foo.rng in %s on line %d

Warning: DOMDocument::relaxNGValidate(): Invalid RelaxNG in %s on line %d
bool(false)
