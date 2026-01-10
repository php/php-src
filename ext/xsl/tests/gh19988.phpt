--TEST--
GH-19988 (zend_string_init with NULL pointer in simplexml (UB))
--EXTENSIONS--
simplexml
xsl
--CREDITS--
YuanchengJiang
--FILE--
<?php
$sxe = simplexml_load_file(__DIR__ . '/53965/collection.xml');
$processor = new XSLTProcessor;
$dom = new DOMDocument;
$dom->load(__DIR__ . '/53965/collection.xsl');
$processor->importStylesheet($dom);
$result = $processor->transformToDoc($sxe, SimpleXMLElement::class);
var_dump($result->getName());
?>
--EXPECT--
string(0) ""
