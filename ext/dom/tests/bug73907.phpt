--TEST--
Bug #73907 nextSibling property not included in var_dump of DOMNode
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$xmlString = '<?xml version="1.0" encoding="utf-8" ?>
<root>
</root>';

$doc = new DOMDocument();
$doc->loadXML($xmlString);
$attr = $doc->documentElement;

var_dump($attr);
?>
--EXPECTF--
object(DOMElement)#%d (%d) {%A
  ["nextSibling"]=>
  NULL
%A}
