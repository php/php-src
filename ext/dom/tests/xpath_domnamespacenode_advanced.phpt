--TEST--
DOMXPath::query() can return DOMNodeList with DOMNameSpaceNode items - advanced variation
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<'XML'
<root xmlns:foo="http://example.com/foo" xmlns:bar="http://example.com/bar">
    <child xmlns:baz="http://example.com/baz">Hello PHP!</child>
</root>
XML);

$xpath = new DOMXPath($dom);
$query = '//namespace::*';

echo "=== All namespace declarations ===\n";

foreach ($xpath->query($query) as $attribute) {
    assert (!is_null($attribute->parentNode));
    echo $attribute->nodeName . ' = ' . $attribute->nodeValue . PHP_EOL;
    var_dump($attribute->parentNode->tagName);
}

echo "=== All namespace declarations with removal attempt ===\n";

foreach ($xpath->query($query) as $attribute) {
    if (is_null($attribute->parentNode)) {
        echo "Skipping null parent of attribute: ", $attribute->nodeName, " ", $attribute->namespaceURI, "\n";
        echo "---\n";
        continue;
    }
    echo $attribute->namespaceURI, "\n";
    echo "Before: ", $attribute->nodeName, " ", $attribute->parentNode->tagName, "\n";
    // Second & third attempt should fail because it's no longer in the document
    try {
        $attribute->parentNode->remove();
    } catch (\DOMException $e) {
        echo $e->getMessage(), "\n";
    }
    // However, it should not cause a use-after-free
    // Note: not connected, but it still has the element as the parent
    echo "parent NULL: ";
    var_dump(is_null($attribute->parentNode));
    echo "isConnected: ";
    var_dump($attribute->isConnected);
    echo "---\n";
}

?>
--EXPECT--
=== All namespace declarations ===
xmlns:xml = http://www.w3.org/XML/1998/namespace
string(4) "root"
xmlns:bar = http://example.com/bar
string(4) "root"
xmlns:foo = http://example.com/foo
string(4) "root"
xmlns:xml = http://www.w3.org/XML/1998/namespace
string(5) "child"
xmlns:bar = http://example.com/bar
string(5) "child"
xmlns:foo = http://example.com/foo
string(5) "child"
xmlns:baz = http://example.com/baz
string(5) "child"
=== All namespace declarations with removal attempt ===
http://www.w3.org/XML/1998/namespace
Before: xmlns:xml root
parent NULL: bool(false)
isConnected: bool(false)
---
http://example.com/bar
Before: xmlns:bar root
Not Found Error
parent NULL: bool(false)
isConnected: bool(false)
---
http://example.com/foo
Before: xmlns:foo root
Not Found Error
parent NULL: bool(false)
isConnected: bool(false)
---
http://www.w3.org/XML/1998/namespace
Before: xmlns:xml child
parent NULL: bool(false)
isConnected: bool(false)
---
Skipping null parent of attribute: xmlns:bar http://example.com/bar
---
Skipping null parent of attribute: xmlns:foo http://example.com/foo
---
http://example.com/baz
Before: xmlns:baz child
Not Found Error
parent NULL: bool(false)
isConnected: bool(false)
---
