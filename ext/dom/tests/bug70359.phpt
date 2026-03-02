--TEST--
Bug #70359 (print_r() on DOMAttr causes Segfault in php_libxml_node_free_list())
--EXTENSIONS--
dom
--FILE--
<?php

echo "-- Test without parent --\n";

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9" xmlns:xsi="fooooooooooooooooooooo"/>
XML);
$spaceNode = $dom->documentElement->getAttributeNode('xmlns');
print_r($spaceNode);

echo "-- Test with parent and non-ns attribute --\n";

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
<url xmlns:xsi="fooooooooooooooooooooo" myattrib="bar"/>
</urlset>
XML);
$spaceNode = $dom->documentElement->firstElementChild->getAttributeNode('myattrib');
var_dump($spaceNode->nodeType);
var_dump($spaceNode->nodeValue);

$dom->documentElement->firstElementChild->remove();
var_dump($spaceNode->parentNode);

echo "-- Test with parent and ns attribute --\n";

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
<url xmlns:xsi="fooooooooooooooooooooo" myattrib="bar"/>
</urlset>
XML);
$spaceNode = $dom->documentElement->firstElementChild->getAttributeNode('xmlns:xsi');
print_r($spaceNode);

$dom->documentElement->firstElementChild->remove();
var_dump($spaceNode->parentNode->nodeName); // Shouldn't crash

?>
--EXPECT--
-- Test without parent --
DOMNameSpaceNode Object
(
    [nodeName] => xmlns
    [nodeValue] => http://www.sitemaps.org/schemas/sitemap/0.9
    [nodeType] => 18
    [prefix] => 
    [localName] => xmlns
    [namespaceURI] => http://www.sitemaps.org/schemas/sitemap/0.9
    [isConnected] => 1
    [ownerDocument] => (object value omitted)
    [parentNode] => (object value omitted)
    [parentElement] => (object value omitted)
)
-- Test with parent and non-ns attribute --
int(2)
string(3) "bar"
NULL
-- Test with parent and ns attribute --
DOMNameSpaceNode Object
(
    [nodeName] => xmlns:xsi
    [nodeValue] => fooooooooooooooooooooo
    [nodeType] => 18
    [prefix] => xsi
    [localName] => xsi
    [namespaceURI] => fooooooooooooooooooooo
    [isConnected] => 1
    [ownerDocument] => (object value omitted)
    [parentNode] => (object value omitted)
    [parentElement] => (object value omitted)
)
string(3) "url"
