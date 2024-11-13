--TEST--
SimpleXML and template content
--EXTENSIONS--
dom
simplexml
--SKIPIF--
<?php
if (!PHP_DEBUG) { die ("skip only for debug build"); }
?>
--FILE--
<?php
$dom = Dom\HTMLDocument::createFromString('<template>foo<template>nested</template></template>', LIBXML_NOERROR);
$head = $dom->head;
$head_sxe = simplexml_import_dom($head);
var_dump($head_sxe);
var_dump($dom->debugGetTemplateCount());
unset($head_sxe->template);
var_dump($head_sxe);
var_dump($dom->debugGetTemplateCount());
?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  ["template"]=>
  object(SimpleXMLElement)#%d (0) {
  }
}
int(2)
object(SimpleXMLElement)#%d (0) {
}
int(0)
