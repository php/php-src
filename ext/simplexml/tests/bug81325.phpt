--TEST--
BUg #81325 (segfault in zif_simplexml_import_dom)
--EXTENSIONS--
simplexml
dom
--FILE--
<?php
$dom = new DOMDocument;
$dom->loadXML("foo");
$xml = simplexml_import_dom($dom);
?>
Done
--EXPECTF--
Warning: DOMDocument::loadXML(): Start tag expected%s

Warning: simplexml_import_dom(): Invalid Nodetype%s
Done
