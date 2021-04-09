--TEST--
Bug #78025 (segfault when accessing properties of DOMDocumentType)
--EXTENSIONS--
dom
--FILE--
<?php
$htm = "<!DOCTYPE><html></html>";
$dom = new DOMDocument;
$dom->loadHTML($htm);
var_dump($dom->doctype->name);
?>
--EXPECTF--
Warning: DOMDocument::loadHTML(): htmlParseDocTypeDecl : no DOCTYPE name ! in Entity, line: 1 in %s on line %d
string(0) ""
