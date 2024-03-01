--TEST--
compareDocumentPosition: attribute order for same element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container a="b" c="d" e="f" />
XML);

$attrs = $dom->documentElement->attributes;
for ($i = 0; $i <= 2; $i++) {
    for ($j = $i + 1; $j <= $i + 2; $j++) {
        echo "--- Compare $i and ", ($j % 3), " ---\n";
        if ($i < ($j % 3)) {
            $expected = DOMNode::DOCUMENT_POSITION_FOLLOWING | DOMNode::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC;
        } else {
            $expected = DOMNode::DOCUMENT_POSITION_PRECEDING | DOMNode::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC;
        }
        var_dump($attrs[$i]->compareDocumentPosition($attrs[$j % 3]) === $expected);
    }
}

?>
--EXPECT--
--- Compare 0 and 1 ---
bool(true)
--- Compare 0 and 2 ---
bool(true)
--- Compare 1 and 2 ---
bool(true)
--- Compare 1 and 0 ---
bool(true)
--- Compare 2 and 0 ---
bool(true)
--- Compare 2 and 1 ---
bool(true)
