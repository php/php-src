--TEST--
Test DOM\XPath::query() with registering a context node's in-scope namespaces
--EXTENSIONS--
dom
--FILE--
<?php

function dump($xpath, $query, $context) {
    echo "--- $query ---\n";
    $nodes = $xpath->query($query, $context);
    foreach ($nodes as $node) {
        echo "nodeName: ";
        var_dump($node->nodeName);
        echo "prefix: ";
        var_dump($node->prefix);
        echo "namespaceURI: ";
        var_dump($node->namespaceURI);
    }
    if (count($nodes) === 0) {
        echo "No nodes found\n";
    }
}

$dom = DOM\XMLDocument::createFromString(<<<XML
<a xmlns="urn:a" xmlns:d="urn:d">
    <b:b xmlns:b="urn:b">
        <c:c1 xmlns:c="urn:c1">
            <b:bar/>
            <c:c2 xmlns:c="urn:c2"/>
            <c:c3 xmlns:c="urn:c3"/>
            <c:c4 xmlns:c="urn:c1"/>
            <d:d/>
            <a/>
        </c:c1>
    </b:b>
</a>
XML);

$c1 = $dom->getElementsByTagNameNS('*', 'c1')->item(0);
$c2 = $dom->getElementsByTagNameNS('*', 'c1')->item(0);

$xpath = new DOM\XPath($dom);

echo "=== Tests with c1 ===\n\n";

dump($xpath, '//b:bar', $c1);
dump($xpath, '//c:c1', $c1);
dump($xpath, '//c:c2', $c1);
dump($xpath, '//c:c3', $c1);
dump($xpath, '//c:c4', $c1);
dump($xpath, '//d:d', $c1);
dump($xpath, '//a', $c1);

echo "\n=== Tests with c2 ===\n\n";

dump($xpath, '//c:c1', $c2);
dump($xpath, '//c:c2', $c2);
dump($xpath, '//d:d', $c2);

?>
--EXPECT--
=== Tests with c1 ===

--- //b:bar ---
nodeName: string(5) "b:bar"
prefix: string(1) "b"
namespaceURI: string(5) "urn:b"
--- //c:c1 ---
nodeName: string(4) "c:c1"
prefix: string(1) "c"
namespaceURI: string(6) "urn:c1"
--- //c:c2 ---
No nodes found
--- //c:c3 ---
No nodes found
--- //c:c4 ---
nodeName: string(4) "c:c4"
prefix: string(1) "c"
namespaceURI: string(6) "urn:c1"
--- //d:d ---
nodeName: string(3) "d:d"
prefix: string(1) "d"
namespaceURI: string(5) "urn:d"
--- //a ---
No nodes found

=== Tests with c2 ===

--- //c:c1 ---
nodeName: string(4) "c:c1"
prefix: string(1) "c"
namespaceURI: string(6) "urn:c1"
--- //c:c2 ---
No nodes found
--- //d:d ---
nodeName: string(3) "d:d"
prefix: string(1) "d"
namespaceURI: string(5) "urn:d"
