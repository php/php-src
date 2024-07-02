--TEST--
DOM\Element::getDescendantNamespaces()
--EXTENSIONS--
dom
--FILE--
<?php

function dump($dom, $name) {
    echo "\n=== $name ===\n";
    $list = $dom->getElementsByTagName($name)[0]->getDescendantNamespaces();
    foreach ($list as $entry) {
        echo "prefix: ";
        var_dump($entry->prefix);
        echo "namespaceURI: ";
        var_dump($entry->namespaceURI);
        echo "element->nodeName: ";
        var_dump($entry->element->nodeName);
        echo "---\n";
    }
}

$dom = DOM\XMLDocument::createFromString(<<<XML
<root xmlns="urn:a">
    <child xmlns="">
        <c:child xmlns:c="urn:c"/>
    </child>
    <b:sibling xmlns:b="urn:b" xmlns:d="urn:d" d:foo="bar">
        <d:child xmlns:d="urn:d2"/>
    </b:sibling>
</root>
XML);

dump($dom, 'c:child');
dump($dom, 'child');
dump($dom, 'b:sibling');
dump($dom, 'd:child');
dump($dom, 'root');

?>
--EXPECT--
=== c:child ===
prefix: string(1) "c"
namespaceURI: string(5) "urn:c"
element->nodeName: string(7) "c:child"
---

=== child ===
prefix: string(1) "c"
namespaceURI: string(5) "urn:c"
element->nodeName: string(7) "c:child"
---

=== b:sibling ===
prefix: NULL
namespaceURI: string(5) "urn:a"
element->nodeName: string(9) "b:sibling"
---
prefix: string(1) "b"
namespaceURI: string(5) "urn:b"
element->nodeName: string(9) "b:sibling"
---
prefix: string(1) "d"
namespaceURI: string(5) "urn:d"
element->nodeName: string(9) "b:sibling"
---
prefix: NULL
namespaceURI: string(5) "urn:a"
element->nodeName: string(7) "d:child"
---
prefix: string(1) "b"
namespaceURI: string(5) "urn:b"
element->nodeName: string(7) "d:child"
---
prefix: string(1) "d"
namespaceURI: string(6) "urn:d2"
element->nodeName: string(7) "d:child"
---

=== d:child ===
prefix: NULL
namespaceURI: string(5) "urn:a"
element->nodeName: string(7) "d:child"
---
prefix: string(1) "b"
namespaceURI: string(5) "urn:b"
element->nodeName: string(7) "d:child"
---
prefix: string(1) "d"
namespaceURI: string(6) "urn:d2"
element->nodeName: string(7) "d:child"
---

=== root ===
prefix: NULL
namespaceURI: string(5) "urn:a"
element->nodeName: string(4) "root"
---
prefix: string(1) "c"
namespaceURI: string(5) "urn:c"
element->nodeName: string(7) "c:child"
---
prefix: NULL
namespaceURI: string(5) "urn:a"
element->nodeName: string(9) "b:sibling"
---
prefix: string(1) "b"
namespaceURI: string(5) "urn:b"
element->nodeName: string(9) "b:sibling"
---
prefix: string(1) "d"
namespaceURI: string(5) "urn:d"
element->nodeName: string(9) "b:sibling"
---
prefix: NULL
namespaceURI: string(5) "urn:a"
element->nodeName: string(7) "d:child"
---
prefix: string(1) "b"
namespaceURI: string(5) "urn:b"
element->nodeName: string(7) "d:child"
---
prefix: string(1) "d"
namespaceURI: string(6) "urn:d2"
element->nodeName: string(7) "d:child"
---
