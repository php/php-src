--TEST--
compareDocumentPosition: attribute order for different element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<outer>
    <inner a="b" c="d" e="f" />
    <inner a="b" x="y" />
</outer>
XML);

$attrs1 = $dom->documentElement->firstElementChild->attributes;
$attrs2 = $dom->documentElement->firstElementChild->nextElementSibling->attributes;

foreach ($attrs1 as $attr1) {
    foreach ($attrs2 as $attr2) {
        echo "--- Compare 1->2 {$attr1->name} and {$attr2->name} ---\n";
        var_dump($attr1->compareDocumentPosition($attr2) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
        echo "--- Compare 2->1 {$attr1->name} and {$attr2->name} ---\n";
        var_dump($attr2->compareDocumentPosition($attr1) === DOMNode::DOCUMENT_POSITION_PRECEDING);
    }
}

?>
--EXPECT--
--- Compare 1->2 a and a ---
bool(true)
--- Compare 2->1 a and a ---
bool(true)
--- Compare 1->2 a and x ---
bool(true)
--- Compare 2->1 a and x ---
bool(true)
--- Compare 1->2 c and a ---
bool(true)
--- Compare 2->1 c and a ---
bool(true)
--- Compare 1->2 c and x ---
bool(true)
--- Compare 2->1 c and x ---
bool(true)
--- Compare 1->2 e and a ---
bool(true)
--- Compare 2->1 e and a ---
bool(true)
--- Compare 1->2 e and x ---
bool(true)
--- Compare 2->1 e and x ---
bool(true)
