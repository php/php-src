--TEST--
Test clone with conflicting namespace prefix
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<root>
    <a:root1 xmlns:a="urn:a">
        <a:root2 xmlns:a="urn:b">
            <a:child xmlns:b="urn:b">
                <b:child1 xmlns:a="urn:x">bar</b:child1>
            </a:child>
        </a:root2>
    </a:root1>
</root>
XML);

$dom->documentElement->firstElementChild->setAttributeNS("urn:y", "a:foo", "bar");

echo $dom->saveXML(), "\n";

$clone = clone $dom->documentElement->firstElementChild;
var_dump($clone->nodeName, $clone->namespaceURI);

foreach ($clone->attributes as $attr) {
    var_dump($attr->name, $attr->namespaceURI);
}

echo $dom->saveXML($clone), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <a:root1 xmlns:a="urn:a" xmlns:ns1="urn:y" ns1:foo="bar">
        <a:root2 xmlns:a="urn:b">
            <a:child xmlns:b="urn:b">
                <b:child1 xmlns:a="urn:x">bar</b:child1>
            </a:child>
        </a:root2>
    </a:root1>
</root>
string(7) "a:root1"
string(5) "urn:a"
string(7) "xmlns:a"
string(29) "http://www.w3.org/2000/xmlns/"
string(5) "a:foo"
string(5) "urn:y"
<a:root1 xmlns:a="urn:a" xmlns:ns1="urn:y" ns1:foo="bar">
        <a:root2 xmlns:a="urn:b">
            <a:child xmlns:b="urn:b">
                <b:child1 xmlns:a="urn:x">bar</b:child1>
            </a:child>
        </a:root2>
    </a:root1>
