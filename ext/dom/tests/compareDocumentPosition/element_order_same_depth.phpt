--TEST--
compareDocumentPosition: element order at the same tree depth
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <div>
        <div>
            <strong/>
            <p>foo</p>
        </div>
    </div>
    <div>
        <div>
            <p>bar</p>
            <strong/>
        </div>
    </div>
</container>
XML);

$xpath = new DOMXPath($dom);
$query = $xpath->query("//p");
$foo = $query->item(0);
$bar = $query->item(1);

echo "--- strong & foo ---\n";
var_dump($foo->previousElementSibling->compareDocumentPosition($foo) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
var_dump($foo->compareDocumentPosition($foo->previousElementSibling) === DOMNode::DOCUMENT_POSITION_PRECEDING);

echo "--- strong & bar ---\n";
var_dump($bar->nextElementSibling->compareDocumentPosition($bar) === DOMNode::DOCUMENT_POSITION_PRECEDING);
var_dump($bar->compareDocumentPosition($bar->nextElementSibling) === DOMNode::DOCUMENT_POSITION_FOLLOWING);

for ($i = 0; $i < 3; $i++) {
    echo "--- Check on depth $i ---\n";
    var_dump($foo->compareDocumentPosition($bar) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
    var_dump($bar->compareDocumentPosition($foo) === DOMNode::DOCUMENT_POSITION_PRECEDING);
    $foo = $foo->parentElement;
    $bar = $bar->parentElement;
}

?>
--EXPECT--
--- strong & foo ---
bool(true)
bool(true)
--- strong & bar ---
bool(true)
bool(true)
--- Check on depth 0 ---
bool(true)
bool(true)
--- Check on depth 1 ---
bool(true)
bool(true)
--- Check on depth 2 ---
bool(true)
bool(true)
