--TEST--
DOMNamedNodeMap iterator key() on an empty attribute map
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<root/>');
$it = $doc->documentElement->attributes->getIterator();
echo "DOMNamedNodeMap valid: ";
var_dump($it->valid());
echo "DOMNamedNodeMap key: ";
var_dump($it->key());
echo "DOMNamedNodeMap current: ";
var_dump($it->current());

$doc = Dom\XMLDocument::createFromString('<root/>');
$it = $doc->documentElement->attributes->getIterator();
echo "Dom\NamedNodeMap key: ";
var_dump($it->key());

?>
--EXPECT--
DOMNamedNodeMap valid: bool(false)
DOMNamedNodeMap key: NULL
DOMNamedNodeMap current: NULL
Dom\NamedNodeMap key: NULL
