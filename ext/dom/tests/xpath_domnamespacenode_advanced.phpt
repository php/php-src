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

echo "-- All namespace attributes --\n";

foreach ($xpath->query($query) as $attribute) {
    echo $attribute->nodeName . ' = ' . $attribute->nodeValue . PHP_EOL;
    var_dump($attribute->parentNode->tagName);
}

echo "-- All namespace attributes with removal attempt --\n";

foreach ($xpath->query($query) as $attribute) {
    echo "Before: ", $attribute->parentNode->tagName, "\n";
    // Second & third attempt should fail because it's no longer in the document
    try {
        $attribute->parentNode->remove();
    } catch (\DOMException $e) {
        echo $e->getMessage(), "\n";
    }
    // However, it should not cause a use-after-free
    echo "After: ", $attribute->parentNode->tagName, "\n";
}

?>
--EXPECT--
-- All namespace attributes --
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
-- All namespace attributes with removal attempt --
Before: root
After: root
Before: root
Not Found Error
After: root
Before: root
Not Found Error
After: root
Before: child
After: child
Before: child
Not Found Error
After: child
Before: child
Not Found Error
After: child
Before: child
Not Found Error
After: child
