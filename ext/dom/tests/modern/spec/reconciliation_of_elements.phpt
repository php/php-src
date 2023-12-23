--TEST--
Reconciliation of elements
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

$root = $dom->documentElement;
$a_child = $dom->getElementsByTagName("a:child")[0];

$root->append($a_child);

echo $dom->saveXML();

// Trigger reconciliation via detach logic in ext/libxml
$root->remove();
unset($root);

echo $dom->saveXML($a_child);

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:xmlns="http://www.w3.org/2000/xmlns/">
    <a:root1 xmlns:a="urn:a" xmlns:a="urn:a">
        <a:root2 xmlns:a="urn:b" xmlns:a="urn:b">
            
        </a:root2>
    </a:root1>
<a:child xmlns:a="urn:b" xmlns:b="urn:b" xmlns:b="urn:b">
                <b:child1 xmlns:a="urn:x" xmlns:a="urn:x">bar</b:child1>
            </a:child></root>
<a:child xmlns:xmlns="http://www.w3.org/2000/xmlns/" xmlns:a="urn:b" xmlns:b="urn:b" xmlns:b="urn:b">
                <b:child1 xmlns:a="urn:x" xmlns:a="urn:x">bar</b:child1>
            </a:child>
