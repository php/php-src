--TEST--
Renaming an attribute node to a name that already exists
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<!DOCTYPE root [
    <!ELEMENT implied-attribute ANY>
    <!ATTLIST implied-attribute hello CDATA #FIXED "world">
]>
<root a="b" c="d" xmlns:ns1="urn:a" ns1:foo="bar">
    <implied-attribute my-attr="x"/>
    <implied-attribute my-attr="x"/>
</root>
XML, LIBXML_DTDATTR);

$root = $dom->documentElement;
try {
    $root->attributes[0]->rename(NULL, 'c');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $root->attributes[0]->rename(NULL, 'c');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $root->attributes[1]->rename(NULL, 'a');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $root->attributes[1]->rename('urn:a', 'foo');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $root->attributes[3]->rename('', 'a');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $root->firstElementChild->attributes[0]->rename(NULL, 'hello');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $root->firstElementChild->attributes[1]->rename(NULL, 'my-attr');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

// This is here to validate that nothing actually changed
echo $dom->saveXML();

?>
--EXPECT--
An attribute with the given name in the given namespace already exists
An attribute with the given name in the given namespace already exists
An attribute with the given name in the given namespace already exists
An attribute with the given name in the given namespace already exists
An attribute with the given name in the given namespace already exists
An attribute with the given name in the given namespace already exists
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
<!ELEMENT implied-attribute ANY>
<!ATTLIST implied-attribute hello CDATA #FIXED "world">
]>
<root xmlns:ns1="urn:a" a="b" c="d" ns1:foo="bar">
    <implied-attribute my-attr="x" hello="world"/>
    <implied-attribute my-attr="x" hello="world"/>
</root>
