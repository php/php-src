--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - toggle variation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createFromString('<p id="test">foo</p>', LIBXML_NOERROR | LIBXML_HTML_NOIMPLIED);
var_dump($dom->getElementById('test')?->nodeName);
$dom->documentElement->toggleAttribute('id');
var_dump($dom->getElementById('test')?->nodeName);
$dom->documentElement->toggleAttribute('id');
var_dump($dom->getElementById('test')?->nodeName);
?>
--EXPECT--
string(1) "P"
NULL
NULL
