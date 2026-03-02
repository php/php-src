--TEST--
Cloning node lists, maps, and collections should fail
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<root a="1"><a/></root>');
try {
    clone $dom->documentElement->attributes;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    clone $dom->documentElement->childNodes;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$dom = Dom\XMLDocument::createFromString('<!DOCTYPE root [<!ENTITY foo "">]><root a="1"><a/></root>');
try {
    clone $dom->documentElement->attributes;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    clone $dom->documentElement->childNodes;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    clone $dom->documentElement->children;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    clone $dom->doctype->entities;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Trying to clone an uncloneable object of class DOMNamedNodeMap
Trying to clone an uncloneable object of class DOMNodeList
Trying to clone an uncloneable object of class Dom\NamedNodeMap
Trying to clone an uncloneable object of class Dom\NodeList
Trying to clone an uncloneable object of class Dom\HTMLCollection
Trying to clone an uncloneable object of class Dom\DtdNamedNodeMap
