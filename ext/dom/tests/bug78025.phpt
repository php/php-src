--TEST--
Bug #78025 (segfault when accessing properties of DOMDocumentType)
--EXTENSIONS--
dom
--FILE--
<?php
$htm = "<!DOCTYPE><html></html>";
$dom = new DOMDocument;
$dom->loadHTML($htm, LIBXML_NOERROR);
var_dump($dom->doctype->name);
?>
--EXPECTF--
string(0) ""
