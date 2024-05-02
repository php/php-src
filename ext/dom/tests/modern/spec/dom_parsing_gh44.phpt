--TEST--
DOM-Parsing GH-44 (It's possible for 'generate a prefix' algorithm to generate a prefix conflicting with an existing one)
--EXTENSIONS--
dom
--FILE--
<?php

$root = DOM\XMLDocument::createFromString('<root xmlns:ns2="uri2"><child xmlns:ns1="uri1" xmlns:a0="uri1" xmlns:NS1="uri1"/></root>')->documentElement;
$root->firstChild->setAttributeNS('uri3', 'attr1', 'value1');
echo $root->ownerDocument->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:ns2="uri2"><child xmlns:ns1="uri1" xmlns:a0="uri1" xmlns:NS1="uri1" xmlns:ns2="uri3" ns2:attr1="value1"/></root>
