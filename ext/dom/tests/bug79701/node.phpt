--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - attribute node variation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
$element = $dom->createElement('foo');
$dom->append($element);
$attr = $dom->createAttribute('id');
$attr->value = 'test';
var_dump($dom->getElementById('test')?->nodeName);
$element->setAttributeNode($attr);
var_dump($dom->getElementById('test')?->nodeName);
?>
--EXPECT--
NULL
string(3) "FOO"
