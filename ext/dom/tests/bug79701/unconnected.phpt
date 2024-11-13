--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - unconnected variation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
$element = $dom->createElement('foo');
$element->setAttribute('id', 'test');
var_dump($dom->getElementById('test')?->nodeName);
$dom->append($element);
var_dump($dom->getElementById('test')?->nodeName);
?>
--EXPECT--
NULL
string(3) "FOO"
