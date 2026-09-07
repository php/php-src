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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    clone $dom->documentElement->childNodes;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$dom = Dom\XMLDocument::createFromString('<!DOCTYPE root [<!ENTITY foo "">]><root a="1"><a/></root>');
try {
    clone $dom->documentElement->attributes;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    clone $dom->documentElement->childNodes;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    clone $dom->documentElement->children;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    clone $dom->doctype->entities;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class DOMNamedNodeMap
Error: Trying to clone an uncloneable object of class DOMNodeList
Error: Trying to clone an uncloneable object of class Dom\NamedNodeMap
Error: Trying to clone an uncloneable object of class Dom\NodeList
Error: Trying to clone an uncloneable object of class Dom\HTMLCollection
Error: Trying to clone an uncloneable object of class Dom\DtdNamedNodeMap
