--TEST--
compareDocumentPosition: element order at a different tree depth
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <div>
        <p>foo</p>
    </div>
    <div>
        <div>
            <p>bar</p>
        </div>
    </div>
</container>
XML);

$xpath = new DOMXPath($dom);
$query = $xpath->query("//p");
$foo = $query->item(0);
$bar = $query->item(1);

for ($i = 0; $i < 2; $i++) {
    echo "--- Check on depth $i ---\n";
    var_dump($foo->compareDocumentPosition($bar) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
    var_dump($bar->compareDocumentPosition($foo) === DOMNode::DOCUMENT_POSITION_PRECEDING);
    $foo = $foo->parentElement;
    $bar = $bar->parentElement;
}

echo "--- One contains the other at depth 2 ---\n";
var_dump(boolval($foo->compareDocumentPosition($bar) & DOMNode::DOCUMENT_POSITION_CONTAINED_BY));
var_dump(boolval($bar->compareDocumentPosition($foo) & DOMNode::DOCUMENT_POSITION_CONTAINS));

?>
--EXPECT--
--- Check on depth 0 ---
bool(true)
bool(true)
--- Check on depth 1 ---
bool(true)
bool(true)
--- One contains the other at depth 2 ---
bool(true)
bool(true)
