--TEST--
Bug #50989  add support LIBXML_NOXMLDECL  for DOMDocument::saveXML()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$dom = new DomDocument();
$dom->loadXML("<foo />");

print $dom->saveXML(null,LIBXML_NOXMLDECL);
--EXPECT--
<foo/>