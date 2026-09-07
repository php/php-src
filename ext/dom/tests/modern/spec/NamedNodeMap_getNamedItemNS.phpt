--TEST--
getNamedItemNS() with an empty URI must look up the null namespace
--EXTENSIONS--
dom
--FILE--
<?php
$d = new DOMDocument();
$d->loadXML('<root xmlns:q="urn:q" bar="no-ns" q:bar="ns"/>');
$a = $d->documentElement->attributes->getNamedItemNS('', 'bar');
var_dump($a === null ? null : $a->nodeValue);
$b = $d->documentElement->attributes->getNamedItemNS('urn:q', 'bar');
var_dump($b === null ? null : $b->nodeValue);
$d2 = Dom\XMLDocument::createFromString('<root xmlns:q="urn:q" bar="no-ns" q:bar="ns"/>');
$a2 = $d2->documentElement->attributes->getNamedItemNS('', 'bar');
var_dump($a2 === null ? null : $a2->nodeValue);
var_dump($d2->documentElement->hasAttributeNS('', 'bar'));
$c = $d2->documentElement->attributes->getNamedItemNS('urn:q', 'bar');
var_dump($c === null ? null : $c->nodeValue);
?>
--EXPECT--
NULL
string(2) "ns"
string(5) "no-ns"
bool(true)
string(2) "ns"
