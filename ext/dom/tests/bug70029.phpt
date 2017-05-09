--TEST--
Bug #70029 (nodeValue of DOMElement list content of children nodes)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXml('<tag><childtag>myString</childtag></tag>');
var_dump($doc->documentElement->nodeValue);
?>
--EXPECT--
NULL
