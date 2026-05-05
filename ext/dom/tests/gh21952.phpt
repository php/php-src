--TEST--
GH-21952 (UAF in php_dom_object_get_data when DOMNotation outlives owning DOCTYPE)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<'XML'
<!DOCTYPE books [
<!NOTATION myNotation SYSTEM "test.dtd">
]>
<container/>
XML);
$notation = $doc->doctype->notations[0];
var_dump($notation->parentNode);
var_dump($notation->isConnected);
$doc->removeChild($doc->doctype);
var_dump($notation->nodeName);
var_dump($notation->systemId);
var_dump($notation->parentNode);
var_dump($notation->isConnected);
?>
--EXPECT--
NULL
bool(false)
string(10) "myNotation"
string(8) "test.dtd"
NULL
bool(false)
